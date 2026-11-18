#include "enginepch.h"
//#include "engine/graphics/Renderpass/Wireframepass.h"
//#include "engine/graphics/mesh.h"
//#include "engine/graphics/utility.h"
//#include "engine/graphics/graphics_context.h"
//#include "engine/graphics/renderer/render_pipeline_layouts.h"
//#include "engine/asset/asset_manager.h"
//#include "engine/utility/file.h"
//
//namespace engine
//{
//	namespace
//	{
//		struct WireframeUniform
//		{
//			glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
//		};
//		static_assert(sizeof(WireframeUniform) % 16 == 0);
//
//		wgpu::RenderPipeline m_wireframePipeline;
//		wgpu::BindGroup m_shadowBindGroup;
//		wgpu::Buffer m_uniformBuffer;
//
//		WireframeUniform m_uniformData;
//	}
//
//	Wireframepass::Wireframepass()
//	{
//		wgpu::BindGroupLayoutEntry entry;
//		entry.binding = 0;
//		entry.visibility = wgpu::ShaderStage::Fragment;
//		entry.buffer.type = wgpu::BufferBindingType::Uniform;
//		entry.buffer.minBindingSize = sizeof(WireframeUniform);
//		entry.buffer.hasDynamicOffset = false;
//
//		wgpu::BindGroupLayoutDescriptor desc;
//		desc.label = "WireframeBindGroupLayout";
//		desc.entryCount = 1;
//		desc.entries = &entry;
//
//		wgpu::BindGroupLayout bindGroupLayout = graphicsContext::GetDevice().CreateBindGroupLayout(&desc);
//
//		wgpu::BufferDescriptor bufferDesc;
//		bufferDesc.label = "WireframeUniformBuffer";
//		bufferDesc.size = 1024 * 256 * sizeof(WireframeUniform); //1024 max entities, 256 bytes alignment
//		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
//
//		m_uniformBuffer = graphicsContext::GetDevice().CreateBuffer(&bufferDesc);
//
//		wgpu::BindGroupEntry bindGroupEntry;
//		bindGroupEntry.binding = 0;
//		bindGroupEntry.buffer = m_uniformBuffer;
//		bindGroupEntry.offset = 0;
//		bindGroupEntry.size = sizeof(WireframeUniform);
//
//		wgpu::BindGroupDescriptor bindGroupDesc;
//		bindGroupDesc.label = "WireframeBindGroup";
//		bindGroupDesc.layout = bindGroupLayout;
//		bindGroupDesc.entryCount = 1;
//		bindGroupDesc.entries = &bindGroupEntry;
//		m_shadowBindGroup = graphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
//
//		wgpu::ShaderModule shaderModule = LoadWGSLShader("resources/shaders/wireframe.wgsl");
//		std::array<wgpu::VertexAttribute, 3> vertexAttribs;
//
//		// Position
//		vertexAttribs[0].shaderLocation = 0; // @location(0)
//		vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
//		vertexAttribs[0].offset = 0;
//
//		// Normal
//		vertexAttribs[1].shaderLocation = 1; // @location(1)
//		vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
//		vertexAttribs[1].offset = 3 * sizeof(float);
//
//		// UV
//		vertexAttribs[2].shaderLocation = 2; // @location(2)
//		vertexAttribs[2].format = wgpu::VertexFormat::Float32x2;
//		vertexAttribs[2].offset = 6 * sizeof(float);
//
//		wgpu::VertexBufferLayout vertexBufferLayout;
//		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
//		vertexBufferLayout.attributes = vertexAttribs.data();
//		vertexBufferLayout.arrayStride = 8 * sizeof(float);
//		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;
//
//		wgpu::RenderPipelineDescriptor pipelineDesc;
//		pipelineDesc.label = "WireframeShaderPipeline";
//
//		pipelineDesc.vertex.bufferCount = 1;
//		pipelineDesc.vertex.buffers = &vertexBufferLayout;
//		pipelineDesc.vertex.module = shaderModule;
//		pipelineDesc.vertex.entryPoint = "vs_main";
//		pipelineDesc.vertex.constantCount = 0;
//		pipelineDesc.vertex.constants = nullptr;
//
//		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::LineList;
//		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
//		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CW;
//		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;
//
//		wgpu::BlendState blendState;
//		blendState.color.srcFactor = wgpu::BlendFactor::One;
//		blendState.color.dstFactor = wgpu::BlendFactor::Zero;
//		blendState.color.operation = wgpu::BlendOperation::Add;
//		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
//		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
//		blendState.alpha.operation = wgpu::BlendOperation::Add;
//
//		wgpu::ColorTargetState colorTarget;
//		colorTarget.format = graphicsContext::GetSurfaceFormat();
//		colorTarget.blend = &blendState;
//		colorTarget.writeMask = wgpu::ColorWriteMask::All;
//
//		wgpu::DepthStencilState depthStencil;
//		depthStencil.format = wgpu::textureFormat::Depth24Plus;
//		depthStencil.depthWriteEnabled = wgpu::OptionalBool::False;
//		depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;
//		depthStencil.depthBias = 0;
//		depthStencil.stencilFront = {};
//		depthStencil.stencilBack = {};
//		depthStencil.stencilReadMask = 0;
//		depthStencil.stencilWriteMask = 0;
//
//		wgpu::FragmentState fragmentState;
//		fragmentState.module = shaderModule;
//		fragmentState.entryPoint = "fs_main";
//		fragmentState.constantCount = 0;
//		fragmentState.constants = nullptr;
//		fragmentState.targetCount = 1;
//		fragmentState.targets = &colorTarget;
//		pipelineDesc.depthStencil = &depthStencil;
//		pipelineDesc.fragment = &fragmentState;
//
//
//		pipelineDesc.multisample.count = 1;
//		pipelineDesc.multisample.mask = ~0u; // Default value for the mask, meaning "all bits on"
//		pipelineDesc.multisample.alphaToCoverageEnabled = false;
//
//		std::array<wgpu::BindGroupLayout, 3> bindGroupLayouts
//		{
//			RenderPipelineLayouts::GetViewLayout(),
//			RenderPipelineLayouts::GetModelLayout(),
//			bindGroupLayout
//		};
//
//		wgpu::PipelineLayoutDescriptor layoutDesc{};
//		layoutDesc.label = "WireframeShaderPipelineLayout";
//		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
//		layoutDesc.bindGroupLayouts = bindGroupLayouts.data();
//		pipelineDesc.layout = graphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);
//
//		m_wireframePipeline = graphicsContext::GetDevice().CreateRenderPipeline(&pipelineDesc);
//	}
//
//	void Wireframepass::SetColor(const glm::vec4& color)
//	{
//		m_uniformData.color = color;
//	}
//
//	void Wireframepass::Execute(wgpu::commandEncoder& encoder, const RenderContext& context)
//	{
//		wgpu::RenderpassColorAttachment color;
//		color.view = context.colorTarget;
//		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
//		color.loadOp = wgpu::LoadOp::Load;
//		color.storeOp = wgpu::StoreOp::Store;
//
//		wgpu::RenderpassDepthStencilAttachment depth;
//		depth.view = context.depthTarget;
//		depth.depthClearValue = 1.0f;
//		depth.depthLoadOp = wgpu::LoadOp::Clear;
//		depth.depthStoreOp = wgpu::StoreOp::Discard;
//		depth.depthReadOnly = false;
//		depth.stencilClearValue = 0;
//		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
//		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
//		depth.stencilReadOnly = true;
//
//		wgpu::RenderpassDescriptor passDescriptor;
//		passDescriptor.label = "WireframeRenderpass";
//		passDescriptor.colorAttachmentCount = 1;
//		passDescriptor.colorAttachments = &color;
//		passDescriptor.depthStencilAttachment = &depth;
//
//		wgpu::RenderpassEncoder pass = encoder.BeginRenderpass(&passDescriptor);
//
//		pass.SetPipeline(m_wireframePipeline);
//		pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);
//		pass.SetBindGroup(1, context.modelBindGroup, 0, nullptr);
//
//		graphicsContext::GetQueue().WriteBuffer(m_uniformBuffer, 0, &m_uniformData, sizeof(WireframeUniform));
//		pass.SetBindGroup(2, m_shadowBindGroup, 0, nullptr);
//
//		engine::Mesh lastMesh;
//
//		size_t i = 0;
//		for (const auto& item : context.drawList.GetItems())
//		{
//			if (item.mesh != lastMesh)
//			{
//				lastMesh = item.mesh;
//				Meshasset& meshasset = engine::assetManager::Getasset<Meshasset>(lastMesh);
//				pass.SetVertexBuffer(0, meshasset.GetVertexBuffer(), 0, meshasset.GetVertexBuffer().GetSize());
//				pass.SetIndexBuffer(meshasset.GetWireIndexBuffer(), wgpu::IndexFormat::Uint32, 0, meshasset.GetWireIndexBuffer().GetSize());
//			}
//
//			pass.DrawIndexed(item.indexCount * 2, 1, item.firstIndex * 2, 0, static_cast<uint32_t>(i));
//			i++;
//		}
//		pass.End();
//	}
//}