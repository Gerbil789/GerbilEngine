#include "enginepch.h"
#include "EntityIdRenderer.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/Shaders/Shader.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine
{
	EntityIdRenderer::EntityIdRenderer(uint32_t width, uint32_t height)
	{
		m_Device = GraphicsContext::GetDevice();
		m_Queue = GraphicsContext::GetQueue();
		CreateBindGroupLayout();
		CreatePipeline();

		Resize(width, height);
	}

	void EntityIdRenderer::Resize(uint32_t width, uint32_t height)
	{
		// Color texture
		wgpu::TextureDescriptor colorTextureDesc{};
		colorTextureDesc.label = { "EntityIdRendererColorTexture", WGPU_STRLEN };
		colorTextureDesc.dimension = wgpu::TextureDimension::_2D;
		colorTextureDesc.format = wgpu::TextureFormat::R32Uint;
		colorTextureDesc.size = { width, height, 1 };
		colorTextureDesc.mipLevelCount = 1;
		colorTextureDesc.sampleCount = 1;
		colorTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		m_Texture = m_Device.createTexture(colorTextureDesc);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.label = { "EntityIdRendererTextureView", WGPU_STRLEN };
		viewDesc.dimension = WGPUTextureViewDimension_2D;
		viewDesc.format = colorTextureDesc.format;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		m_TextureView = m_Texture.createView(viewDesc);

		// Depth texture
		wgpu::TextureDescriptor depthTextureDesc;
		colorTextureDesc.label = { "EntityIdRendererDepthTexture", WGPU_STRLEN };
		depthTextureDesc.dimension = wgpu::TextureDimension::_2D;
		depthTextureDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthTextureDesc.mipLevelCount = 1;
		depthTextureDesc.sampleCount = 1;
		depthTextureDesc.size = { width, height, 1 };
		depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
		depthTextureDesc.viewFormatCount = 1;
		depthTextureDesc.viewFormats = (WGPUTextureFormat*)&wgpu::TextureFormat::Depth24Plus;
		wgpu::Texture depthTexture = m_Device.createTexture(depthTextureDesc);

		wgpu::TextureViewDescriptor depthTextureViewDesc;
		depthTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		depthTextureViewDesc.baseArrayLayer = 0;
		depthTextureViewDesc.arrayLayerCount = 1;
		depthTextureViewDesc.baseMipLevel = 0;
		depthTextureViewDesc.mipLevelCount = 1;
		depthTextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
		depthTextureViewDesc.format = wgpu::TextureFormat::Depth24Plus;
		m_DepthView = depthTexture.createView(depthTextureViewDesc);
	}

	void EntityIdRenderer::Render()
	{
		wgpu::RenderPassColorAttachment colorAttachment{};
		colorAttachment.view = m_TextureView;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		colorAttachment.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };

		wgpu::RenderPassDepthStencilAttachment depthStencilAttachment{};
		depthStencilAttachment.view = m_DepthView;
		depthStencilAttachment.depthClearValue = 1.0f;
		depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
		depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
		depthStencilAttachment.depthReadOnly = false;
		depthStencilAttachment.stencilClearValue = 0;
		depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Undefined;
		depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Undefined;
		depthStencilAttachment.stencilReadOnly = true;

		wgpu::RenderPassDescriptor renderPassDescriptor;
		renderPassDescriptor.label = { "EntityIdRenderPass", WGPU_STRLEN };
		renderPassDescriptor.colorAttachmentCount = 1;
		renderPassDescriptor.colorAttachments = &colorAttachment;
		renderPassDescriptor.depthStencilAttachment = &depthStencilAttachment;

		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = { "RendererCommandEncoder", WGPU_STRLEN };
		wgpu::CommandEncoder encoder = m_Device.createCommandEncoder(encoderDesc);

		wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDescriptor);
		renderPass.setPipeline(m_Pipeline);

		renderPass.setBindGroup(GroupID::Frame, Renderer::GetFrameBindGroup(), 0, nullptr);

		std::vector<Entity> entities = m_Scene->GetEntities<TransformComponent, MeshComponent>();

		uint32_t i = 0;
		for (Entity& entity : entities)
		{
			uint32_t id = entity.GetUUID();

			auto& meshComponent = entity.GetComponent<MeshComponent>();
			auto& mesh = meshComponent.Mesh;

			glm::mat4 modelMatrix = entity.GetComponent<TransformComponent>().GetModelMatrix();

			uint32_t dynamicOffset = i * Renderer::GetModelUniformStride();
			renderPass.setBindGroup(GroupID::Model, Renderer::GetModelBindGroup(), 1, &dynamicOffset);


			uint32_t iDdynamicOffset = i * 256;
			m_Queue.writeBuffer(m_UniformBuffer, iDdynamicOffset, &id, sizeof(id));
			renderPass.setBindGroup(2, m_BindGroup, 1, &iDdynamicOffset);

			renderPass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
			renderPass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, mesh->GetIndexBuffer().getSize());
			renderPass.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);

			i++;
		}

		renderPass.end();
		renderPass.release();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		encoder.release();
		m_Queue.submit(1, &commandBuffer);
	}

	uint32_t EntityIdRenderer::ReadPixel(uint32_t x, uint32_t y)
	{
		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "EntityIdReadbackBuffer", WGPU_STRLEN };
		bufferDesc.size = sizeof(uint32_t);
		bufferDesc.usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst;
		wgpu::Buffer readbackBuffer = m_Device.createBuffer(bufferDesc);

		// Copy from texture to buffer
		wgpu::TexelCopyTextureInfo src{};
		src.texture = m_Texture;
		src.origin = { x, y, 0 };

		wgpu::TexelCopyBufferInfo dst{};
		dst.buffer = readbackBuffer;
		dst.layout.bytesPerRow = 256;
		dst.layout.rowsPerImage = 1;

		wgpu::Extent3D copySize{ 1, 1, 1 };

		wgpu::CommandEncoderDescriptor encoderDesc{};
		encoderDesc.label = { "EntityIdReadbackEncoder", WGPU_STRLEN };
		wgpu::CommandEncoder encoder = m_Device.createCommandEncoder(encoderDesc);

		encoder.copyTextureToBuffer(src, dst, copySize);
		wgpu::CommandBuffer cmdBuffer = encoder.finish();
		encoder.release();
		m_Queue.submit(1, &cmdBuffer);

		wgpu::BufferMapCallbackInfo callbackInfo{};
		callbackInfo.mode = wgpu::CallbackMode::WaitAnyOnly;
		callbackInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2) {
			if (status != wgpu::MapAsyncStatus::Success) {
				LOG_ERROR("Readback buffer map callback: status: {0}, message: {1}", (int)status, message);
			}
		};

		wgpu::Future future = readbackBuffer.mapAsync(wgpu::MapMode::Read, 0, sizeof(uint32_t), callbackInfo);
		wgpu::FutureWaitInfo waitInfo;
		waitInfo.future = future;
		wgpuInstanceWaitAny(GraphicsContext::GetInstance(), 1, &waitInfo, 100000000); // 100000000ns = 0.1s

		const uint8_t* mapped = static_cast<const uint8_t*>(readbackBuffer.getConstMappedRange(0, sizeof(uint32_t)));
		uint32_t id = *reinterpret_cast<const uint32_t*>(mapped);
		readbackBuffer.release();

		return id;
	}

	void EntityIdRenderer::CreateBindGroupLayout()
	{
		wgpu::BindGroupLayoutEntry entry = wgpu::Default;
		entry.binding = 0;
		entry.visibility = wgpu::ShaderStage::Fragment;
		entry.buffer.type = wgpu::BufferBindingType::Uniform;
		entry.buffer.minBindingSize = sizeof(uint32_t);
		entry.buffer.hasDynamicOffset = true;

		wgpu::BindGroupLayoutDescriptor desc{};
		desc.label = { "EntityIdBindGroupLayout", WGPU_STRLEN };
		desc.entryCount = 1;
		desc.entries = &entry;

		m_BindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(desc);

		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "EntityIdUniformBuffer", WGPU_STRLEN };;
		bufferDesc.size = 1024 * sizeof(uint32_t);
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

		m_UniformBuffer = m_Device.createBuffer(bufferDesc);

		wgpu::BindGroupEntry bindGroupEntry{};
		bindGroupEntry.binding = 0;
		bindGroupEntry.buffer = m_UniformBuffer;
		bindGroupEntry.offset = 0;
		bindGroupEntry.size = sizeof(uint32_t);

		wgpu::BindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.label = { "EntityIdBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = m_BindGroupLayout;
		bindGroupDesc.entryCount = 1;
		bindGroupDesc.entries = &bindGroupEntry;
		m_BindGroup = m_Device.createBindGroup(bindGroupDesc);
	}

	void EntityIdRenderer::CreatePipeline()
	{
		wgpu::ShaderModule shaderModule = Shader::LoadShader("resources/shaders/entityId.wgsl");
		std::vector<wgpu::VertexAttribute> vertexAttribs(3);

		// Position
		vertexAttribs[0].shaderLocation = 0; // @location(0)
		vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[0].offset = 0;

		// Normal
		vertexAttribs[1].shaderLocation = 1; // @location(1)
		vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[1].offset = 3 * sizeof(float);

		// UV
		vertexAttribs[2].shaderLocation = 2; // @location(2)
		vertexAttribs[2].format = wgpu::VertexFormat::Float32x2;
		vertexAttribs[2].offset = 6 * sizeof(float);

		wgpu::VertexBufferLayout vertexBufferLayout;

		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
		vertexBufferLayout.attributes = vertexAttribs.data();
		vertexBufferLayout.arrayStride = 8 * sizeof(float);
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;


		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = { "EntityIdShaderPipeline", WGPU_STRLEN };

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN };
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None; //TODO: Add culling later

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = wgpu::TextureFormat::R32Uint;
		//colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		wgpu::DepthStencilState depthStencilState{};
		depthStencilState.depthCompare = wgpu::CompareFunction::Less;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u; // Default value for the mask, meaning "all bits on"
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			Renderer::GetFrameBindGroupLayout(),
			Renderer::GetModelBindGroupLayout(),
			m_BindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "FlatColorShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = 3;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = m_Device.createPipelineLayout(layoutDesc);

		m_Pipeline = m_Device.createRenderPipeline(pipelineDesc);
		shaderModule.release();
	}


}