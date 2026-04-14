#include "EditorPicker.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Utility/File.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Scene.h"
#include <execution>

namespace Editor
{
  EditorPicker::EditorPicker()
  {
    CreateBindGroupLayout();
    CreatePipeline();

    // Create the persistent 8-byte readback buffer
    wgpu::BufferDescriptor bufferDesc{};
    bufferDesc.label = { "EditorPickerReadbackBuffer", WGPU_STRLEN };
    bufferDesc.size = sizeof(Engine::Uuid);
    bufferDesc.usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst;
    m_ReadbackBuffer = Engine::GraphicsContext::GetDevice().createBuffer(bufferDesc);

    Resize(1, 1); // Initialize with dummy size
  }

  EditorPicker::~EditorPicker()
  {
    if (m_ReadbackBuffer) m_ReadbackBuffer.release();
    if (m_UniformBuffer) m_UniformBuffer.release();
  }

  Engine::Uuid EditorPicker::Pick(uint32_t x, uint32_t y, const Engine::RenderContext& context)
  {
    // Out of bounds check
    if (x >= m_Width || y >= m_Height) return Engine::Uuid(0);

    // 1. Create a dedicated command encoder for the pick operation
    wgpu::CommandEncoderDescriptor encoderDesc{};
    encoderDesc.label = { "EditorPickerEncoder", WGPU_STRLEN };
    wgpu::CommandEncoder encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder(encoderDesc);

    // 2. Setup Render Pass
    wgpu::RenderPassColorAttachment colorAttach{};
    colorAttach.view = m_ColorTextureView;
    colorAttach.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    colorAttach.loadOp = wgpu::LoadOp::Clear;
    colorAttach.storeOp = wgpu::StoreOp::Store;
    colorAttach.clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };

    wgpu::RenderPassDepthStencilAttachment depthAttach{};
    depthAttach.view = m_DepthTextureView;
    depthAttach.depthClearValue = 1.0f;
    depthAttach.depthLoadOp = wgpu::LoadOp::Clear;
    depthAttach.depthStoreOp = wgpu::StoreOp::Store;
    depthAttach.depthReadOnly = false;

    wgpu::RenderPassDescriptor passDescriptor{};
    passDescriptor.label = { "EditorPickerPass", WGPU_STRLEN };
    passDescriptor.colorAttachmentCount = 1;
    passDescriptor.colorAttachments = &colorAttach;
    passDescriptor.depthStencilAttachment = &depthAttach;

    wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);
    pass.setPipeline(m_Pipeline);

    pass.setScissorRect(x, y, 1, 1);

    pass.setBindGroup(0, context.viewBindGroup, 0, nullptr);

    Engine::Mesh* currentMesh = nullptr;

    for (const Engine::DrawItem& draw : context.drawList)
    {
      if (!draw.mesh) continue;

      if (draw.mesh != currentMesh)
      {
        currentMesh = draw.mesh;
        pass.setVertexBuffer(0, currentMesh->GetVertexBuffer(), 0, currentMesh->GetVertexBuffer().getSize());
        pass.setIndexBuffer(currentMesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, currentMesh->GetIndexBuffer().getSize());
      }

      uint32_t dynamicOffset = draw.modelIndex * Engine::GraphicsContext::GetUniformBufferOffsetAlignment();
      pass.setBindGroup(1, context.modelBindGroup, 1, &dynamicOffset);

      Engine::Uuid uuid = draw.entity.GetUUID();
      Engine::GraphicsContext::GetQueue().writeBuffer(m_UniformBuffer, dynamicOffset, &uuid, sizeof(Engine::Uuid));
      pass.setBindGroup(2, m_BindGroup, 1, &dynamicOffset);

      const Engine::SubMesh* sub = draw.subMesh;
      pass.drawIndexed(sub->indexCount, 1, sub->firstIndex, 0, 0);
    }

    pass.end();

    // 4. Copy that exact 1 pixel to the readback buffer
    wgpu::TexelCopyTextureInfo src{};
    src.texture = m_ColorTexture;
    src.origin = { x, y, 0 };

    wgpu::TexelCopyBufferInfo dst{};
    dst.buffer = m_ReadbackBuffer;
    dst.layout.bytesPerRow = 256; // WebGPU minimum alignment
    dst.layout.rowsPerImage = 1;

    wgpu::Extent3D copySize{ 1, 1, 1 };
    encoder.copyTextureToBuffer(src, dst, copySize);

    // Submit to GPU immediately
    wgpu::CommandBuffer cmdBuffer = encoder.finish();
    Engine::GraphicsContext::GetQueue().submit(1, &cmdBuffer);

    // 5. Synchronous map and read (Acceptable for an on-click editor action)
    wgpu::BufferMapCallbackInfo callbackInfo{};
    callbackInfo.mode = wgpu::CallbackMode::WaitAnyOnly;
    callbackInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void*, void*) {
      if (status != wgpu::MapAsyncStatus::Success) 
      {
        LOG_ERROR("Picker readback map failed: {}", message.data);
      }
      };

    wgpu::Future future = m_ReadbackBuffer.mapAsync(wgpu::MapMode::Read, 0, sizeof(Engine::Uuid), callbackInfo);

    wgpu::FutureWaitInfo waitInfo;
    waitInfo.future = future;
    wgpuInstanceWaitAny(Engine::GraphicsContext::GetInstance(), 1, &waitInfo, 100000000); // Wait up to 0.1s

    const uint8_t* mapped = static_cast<const uint8_t*>(m_ReadbackBuffer.getConstMappedRange(0, sizeof(Engine::Uuid)));

    Engine::Uuid pickedId(0);
    if (mapped)
    {
      const uint64_t* pixel = reinterpret_cast<const uint64_t*>(mapped);
      pickedId = Engine::Uuid(*pixel);
    }

    m_ReadbackBuffer.unmap(); // Critical: Unmap so it can be used for the next click

    return pickedId;
  }

  void EditorPicker::Resize(uint32_t width, uint32_t height)
  {
    if (width == 0 || height == 0) return;
    m_Width = width;
    m_Height = height;

    // Color Target (Entity IDs)
    wgpu::TextureDescriptor colorDesc{};
    colorDesc.label = { "EditorPickerColorTexture", WGPU_STRLEN };
    colorDesc.dimension = wgpu::TextureDimension::_2D;
    colorDesc.format = wgpu::TextureFormat::RG32Uint;
    colorDesc.size = { width, height, 1 };
    colorDesc.mipLevelCount = 1;
    colorDesc.sampleCount = 1;
    colorDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;

    m_ColorTexture = Engine::GraphicsContext::GetDevice().createTexture(colorDesc);

    wgpu::TextureViewDescriptor colorViewDesc{};
    colorViewDesc.format = colorDesc.format;
		colorViewDesc.arrayLayerCount = 1;
		colorViewDesc.mipLevelCount = 1;
    m_ColorTextureView = m_ColorTexture.createView(colorViewDesc);

    // Dedicated Depth Target
    wgpu::TextureDescriptor depthDesc{};
    depthDesc.label = { "EditorPickerDepthTexture", WGPU_STRLEN };
    depthDesc.dimension = wgpu::TextureDimension::_2D;
    depthDesc.format = wgpu::TextureFormat::Depth24Plus;
    depthDesc.size = { width, height, 1 };
    depthDesc.mipLevelCount = 1;
    depthDesc.sampleCount = 1;
    depthDesc.usage = wgpu::TextureUsage::RenderAttachment;

    m_DepthTexture = Engine::GraphicsContext::GetDevice().createTexture(depthDesc);

    wgpu::TextureViewDescriptor depthViewDesc{};
    depthViewDesc.format = depthDesc.format;
    depthViewDesc.arrayLayerCount = 1;
    depthViewDesc.mipLevelCount = 1;
    m_DepthTextureView = m_DepthTexture.createView(depthViewDesc);
  }

  void EditorPicker::CreateBindGroupLayout()
  {
    wgpu::BindGroupLayoutEntry entry;
    entry.binding = 0;
    entry.visibility = wgpu::ShaderStage::Fragment;
    entry.buffer.type = wgpu::BufferBindingType::Uniform;
    entry.buffer.minBindingSize = sizeof(Engine::Uuid);
    entry.buffer.hasDynamicOffset = true;

    wgpu::BindGroupLayoutDescriptor layoutDesc;
    layoutDesc.label = { "EditorPickerBindGroupLayout", WGPU_STRLEN };
    layoutDesc.entryCount = 1;
    layoutDesc.entries = &entry;

    m_BindGroupLayout = Engine::GraphicsContext::GetDevice().createBindGroupLayout(layoutDesc);

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = { "EditorPickerUniformBuffer", WGPU_STRLEN };
    bufferDesc.size = 1024 * 256 * sizeof(Engine::Uuid);
    bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
    m_UniformBuffer = Engine::GraphicsContext::GetDevice().createBuffer(bufferDesc);

    wgpu::BindGroupEntry bindGroupEntry;
    bindGroupEntry.binding = 0;
    bindGroupEntry.buffer = m_UniformBuffer;
    bindGroupEntry.offset = 0;
    bindGroupEntry.size = sizeof(Engine::Uuid);

    wgpu::BindGroupDescriptor bindGroupDesc;
    bindGroupDesc.label = { "EditorPickerBindGroup", WGPU_STRLEN };
    bindGroupDesc.layout = m_BindGroupLayout;
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &bindGroupEntry;
    m_BindGroup = Engine::GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
  }

  void EditorPicker::CreatePipeline()
  {
    wgpu::ShaderModule shaderModule = Engine::LoadWGSLShader("Resources/Engine/shaders/picker.wgsl");

    std::vector<wgpu::VertexAttribute> vertexAttribs(3);
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
    fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    wgpu::RenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.label = { "EditorPickerPipeline", WGPU_STRLEN };

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN };

    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

    pipelineDesc.depthStencil = &depthStencil;
    pipelineDesc.fragment = &fragmentState;
    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;

    wgpu::BindGroupLayout bindGroupLayouts[] = {
        Engine::Renderer::GetViewLayout(),
        Engine::Renderer::GetModelLayout(),
        m_BindGroupLayout
    };

    wgpu::PipelineLayoutDescriptor layoutDesc{};
    layoutDesc.label = { "EditorPickerPipelineLayout", WGPU_STRLEN };
    layoutDesc.bindGroupLayoutCount = 3;
    layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;

    pipelineDesc.layout = Engine::GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

    m_Pipeline = Engine::GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
  }
}