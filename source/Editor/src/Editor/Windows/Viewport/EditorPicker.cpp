#include "EditorPicker.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Graphics/Utility.h"
#include <ranges>

namespace Editor
{
  EditorPicker::~EditorPicker()
  {
    if (m_ReadbackBuffer) m_ReadbackBuffer = nullptr;
    if (m_IdStorageBuffer) m_IdStorageBuffer = nullptr;
  }

  void EditorPicker::Initialize()
  {
    CreateBindGroupLayout();
    CreatePipeline();

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = "EditorPickerReadbackBuffer";
    bufferDesc.size = sizeof(Engine::Uuid);
    bufferDesc.usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst;
    m_ReadbackBuffer = Engine::GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
  }

  Engine::Uuid EditorPicker::Pick(uint32_t x, uint32_t y)
  {
    if (x >= m_Width || y >= m_Height) return Engine::Uuid{};

    const Engine::RenderContext& context = Editor::editorContext.renderer.GetRenderContext();
    if (context.drawList.size() == 0) return Engine::Uuid{};

    std::vector<Engine::Uuid> entityIds;
    entityIds.reserve(context.drawList.size());

    for (const auto& item : context.drawList.GetItems())
    {
      if (!item.meshId)
      {
        entityIds.push_back(Engine::Uuid{});
        continue;
      }
      entityIds.push_back(item.entityId);
    }

    Engine::GraphicsContext::GetQueue().WriteBuffer(m_IdStorageBuffer, 0, entityIds.data(), entityIds.size() * sizeof(Engine::Uuid));

    wgpu::CommandEncoderDescriptor encoderDesc;
    encoderDesc.label = "EditorPickerEncoder";
    wgpu::CommandEncoder encoder = Engine::GraphicsContext::GetDevice().CreateCommandEncoder(&encoderDesc);

    wgpu::RenderPassColorAttachment colorAttach;
    colorAttach.view = m_ColorTextureView;
    colorAttach.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    colorAttach.loadOp = wgpu::LoadOp::Clear;
    colorAttach.storeOp = wgpu::StoreOp::Store;
    colorAttach.clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };

    wgpu::RenderPassDepthStencilAttachment depthAttach;
    depthAttach.view = m_DepthTextureView;
    depthAttach.depthClearValue = 1.0f;
    depthAttach.depthLoadOp = wgpu::LoadOp::Clear;
    depthAttach.depthStoreOp = wgpu::StoreOp::Store;
    depthAttach.depthReadOnly = false;

    wgpu::RenderPassDescriptor passDescriptor;
    passDescriptor.label = "EditorPickerPass";
    passDescriptor.colorAttachmentCount = 1;
    passDescriptor.colorAttachments = &colorAttach;
    passDescriptor.depthStencilAttachment = &depthAttach;

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDescriptor);
    pass.SetPipeline(m_Pipeline);

    pass.SetScissorRect(x, y, 1, 1);

    pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);
    pass.SetBindGroup(1, context.modelBindGroup, 0, nullptr);
    pass.SetBindGroup(2, m_BindGroup, 0, nullptr);

    Engine::Uuid currentMesh{};

    for (const auto& [i, item] : std::views::enumerate(context.drawList.GetItems()))
    {
      if (item.meshId != currentMesh)
      {
        currentMesh = item.meshId;
				const Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(currentMesh);
        pass.SetVertexBuffer(0, mesh.GetVertexBuffer(), 0, mesh.GetVertexBuffer().GetSize());
        pass.SetIndexBuffer(mesh.GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh.GetIndexBuffer().GetSize());
      }

      pass.DrawIndexed(item.indexCount, 1, item.firstIndex, 0, static_cast<uint32_t>(i));
    }

    pass.End();

    wgpu::TexelCopyTextureInfo src;
    src.texture = m_ColorTexture;
    src.origin = { x, y, 0 };

    wgpu::TexelCopyBufferInfo dst;
    dst.buffer = m_ReadbackBuffer;
    dst.layout.bytesPerRow = 256; // WebGPU minimum alignment
    dst.layout.rowsPerImage = 1;

    wgpu::Extent3D copySize{ 1, 1, 1 };
    encoder.CopyTextureToBuffer(&src, &dst, &copySize);

    wgpu::CommandBuffer cmdBuffer = encoder.Finish();
    Engine::GraphicsContext::GetQueue().Submit(1, &cmdBuffer);

    wgpu::Future future = m_ReadbackBuffer.MapAsync(wgpu::MapMode::Read, 0, sizeof(Engine::Uuid), wgpu::CallbackMode::WaitAnyOnly, 
      [](wgpu::MapAsyncStatus status, wgpu::StringView message)
      {
        if (status != wgpu::MapAsyncStatus::Success) 
        { 
          LOG_ERROR("Picker readback map failed: {}", std::string_view(message)); 
        }
      });

    wgpu::FutureWaitInfo waitInfo{};
    waitInfo.future = future;

    wgpu::WaitStatus waitStatus = Engine::GraphicsContext::GetInstance().WaitAny(1, &waitInfo, 100000000); // 0.1s timeout

    Engine::Uuid pickedId{};

    if (waitStatus == wgpu::WaitStatus::Success && waitInfo.completed)
    {
      const uint64_t* pixel = static_cast<const uint64_t*>(m_ReadbackBuffer.GetConstMappedRange(0, sizeof(Engine::Uuid)));

      if (pixel)
      {
        pickedId = Engine::Uuid{ *pixel };
      }
    }
    else
    {
      LOG_WARNING("Editor Picker GPU readback timed out!");
    }

    m_ReadbackBuffer.Unmap();
    return pickedId;
  }

  void EditorPicker::Resize(uint32_t width, uint32_t height)
  {
    if (width == 0 || height == 0) return;
    m_Width = width;
    m_Height = height;

    // Color Target (Entity IDs)
    wgpu::TextureDescriptor colorDesc;
    colorDesc.label = "EditorPickerColorTexture";
    colorDesc.dimension = wgpu::TextureDimension::e2D;
    colorDesc.format = wgpu::TextureFormat::RG32Uint;
    colorDesc.size = { width, height, 1 };
    colorDesc.mipLevelCount = 1;
    colorDesc.sampleCount = 1;
    colorDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
    m_ColorTexture = Engine::GraphicsContext::GetDevice().CreateTexture(&colorDesc);

    wgpu::TextureViewDescriptor colorViewDesc;
    colorViewDesc.format = colorDesc.format;
		colorViewDesc.arrayLayerCount = 1;
		colorViewDesc.mipLevelCount = 1;
    m_ColorTextureView = m_ColorTexture.CreateView(&colorViewDesc);

    wgpu::TextureDescriptor depthDesc;
    depthDesc.label = "EditorPickerDepthTexture";
    depthDesc.dimension = wgpu::TextureDimension::e2D;
    depthDesc.format = wgpu::TextureFormat::Depth24Plus;
    depthDesc.size = { width, height, 1 };
    depthDesc.mipLevelCount = 1;
    depthDesc.sampleCount = 1;
    depthDesc.usage = wgpu::TextureUsage::RenderAttachment;
    m_DepthTexture = Engine::GraphicsContext::GetDevice().CreateTexture(&depthDesc);

    wgpu::TextureViewDescriptor depthViewDesc;
    depthViewDesc.format = depthDesc.format;
    depthViewDesc.arrayLayerCount = 1;
    depthViewDesc.mipLevelCount = 1;
    m_DepthTextureView = m_DepthTexture.CreateView(&depthViewDesc);
  }

  void EditorPicker::CreateBindGroupLayout()
  {
    wgpu::BindGroupLayoutEntry entry;
    entry.binding = 0;
    entry.visibility = wgpu::ShaderStage::Fragment;
    entry.buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
    entry.buffer.minBindingSize = sizeof(Engine::Uuid);
    entry.buffer.hasDynamicOffset = false;

    wgpu::BindGroupLayoutDescriptor layoutDesc;
    layoutDesc.label = "EditorPickerBindGroupLayout";
    layoutDesc.entryCount = 1;
    layoutDesc.entries = &entry;

    m_BindGroupLayout = Engine::GraphicsContext::GetDevice().CreateBindGroupLayout(&layoutDesc);

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = "EditorPickerUniformBuffer";
    bufferDesc.size = 1024 * 256 * sizeof(Engine::Uuid);
    bufferDesc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
    m_IdStorageBuffer = Engine::GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);

    wgpu::BindGroupEntry bindGroupEntry;
    bindGroupEntry.binding = 0;
    bindGroupEntry.buffer = m_IdStorageBuffer;
    bindGroupEntry.offset = 0;
    bindGroupEntry.size = bufferDesc.size;

    wgpu::BindGroupDescriptor bindGroupDesc;
    bindGroupDesc.label = "EditorPickerBindGroup";
    bindGroupDesc.layout = m_BindGroupLayout;
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &bindGroupEntry;
    m_BindGroup = Engine::GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
  }

  void EditorPicker::CreatePipeline()
  {
    wgpu::ShaderModule shaderModule = Engine::LoadWGSLShader("resources/shaders/picker.wgsl");

    std::array<wgpu::VertexAttribute, 3> vertexAttribs;
    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[0].offset = 0;

    vertexAttribs[1].shaderLocation = 1;
    vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[1].offset = 3 * sizeof(float);

    vertexAttribs[2].shaderLocation = 2;
    vertexAttribs[2].format = wgpu::VertexFormat::Float32x2;
    vertexAttribs[2].offset = 6 * sizeof(float);

    wgpu::VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
    vertexBufferLayout.attributes = vertexAttribs.data();
    vertexBufferLayout.arrayStride = 8 * sizeof(float);
    vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

    wgpu::ColorTargetState colorTarget;
    colorTarget.format = wgpu::TextureFormat::RG32Uint;
    colorTarget.writeMask = wgpu::ColorWriteMask::All;

    wgpu::DepthStencilState depthStencil;
    depthStencil.format = wgpu::TextureFormat::Depth24Plus;
    depthStencil.depthWriteEnabled = wgpu::OptionalBool::True; // MUST write depth for accurate picking
    depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;

    wgpu::FragmentState fragmentState;
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    wgpu::RenderPipelineDescriptor pipelineDesc;
    pipelineDesc.label = "EditorPickerPipeline";

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";

    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

    pipelineDesc.depthStencil = &depthStencil;
    pipelineDesc.fragment = &fragmentState;
    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;

    std::array<wgpu::BindGroupLayout, 3> bindGroupLayouts 
    {
        Engine::RenderPipelineLayouts::GetViewLayout(),
        Engine::RenderPipelineLayouts::GetModelLayout(),
        m_BindGroupLayout
    };

    wgpu::PipelineLayoutDescriptor layoutDesc;
    layoutDesc.label = "EditorPickerPipelineLayout";
    layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
    layoutDesc.bindGroupLayouts = bindGroupLayouts.data();

    pipelineDesc.layout = Engine::GraphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);

    m_Pipeline = Engine::GraphicsContext::GetDevice().CreateRenderPipeline(&pipelineDesc);
  }
}