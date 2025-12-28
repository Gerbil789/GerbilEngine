#include "enginepch.h"
#include "EntityIdPass.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Utils/File.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"

namespace Engine
{
	EntityIdPass::EntityIdPass()
	{
		Resize(1, 1);
		CreateBindGroupLayout();
		CreatePipeline();
	}

	void EntityIdPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList)
	{
		if (context.width != m_Width || context.height != m_Height)
		{
			Resize(context.width, context.height);
		}

		wgpu::RenderPassColorAttachment color{};
		color.view = m_TextureView;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Clear;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };

		wgpu::RenderPassDepthStencilAttachment depth{};
		depth.view = context.depthTarget;
		depth.depthClearValue = 1.0f;
		depth.depthLoadOp = wgpu::LoadOp::Load;
		depth.depthStoreOp = wgpu::StoreOp::Store;
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;


		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = { "EntityIdRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);
		pass.setPipeline(m_EntityIdPipeline);

		pass.setBindGroup(GroupID::Frame, RenderGlobals::GetFrameBindGroup(), 0, nullptr);

		Mesh* currentMesh = nullptr;

		for (const DrawItem& draw : drawList.items)
		{
			if (!draw.mesh) continue;

			if (draw.mesh != currentMesh)
			{
				pass.setVertexBuffer(0, draw.mesh->GetVertexBuffer(), 0, draw.mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(draw.mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, draw.mesh->GetIndexBuffer().getSize());
				currentMesh = draw.mesh;
			}

			uint32_t dynamicOffset = draw.modelIndex * RenderGlobals::GetModelUniformStride();
			pass.setBindGroup(GroupID::Model, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);


			Engine::UUID uuid = draw.entity.GetUUID();
			GraphicsContext::GetQueue().writeBuffer(m_UniformBuffer, dynamicOffset, &uuid, sizeof(Engine::UUID));
			pass.setBindGroup(2, m_BindGroup, 1, &dynamicOffset);

			pass.drawIndexed(currentMesh->GetIndexCount(), 1, 0, 0, 0);
		}

		pass.end();
	}

	Engine::UUID EntityIdPass::ReadPixel(uint32_t x, uint32_t y)
	{
		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "EntityIdReadbackBuffer", WGPU_STRLEN };
		bufferDesc.size = sizeof(Engine::UUID);
		bufferDesc.usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst;
		wgpu::Buffer readbackBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);

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
		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().createCommandEncoder(encoderDesc);

		encoder.copyTextureToBuffer(src, dst, copySize);
		wgpu::CommandBuffer cmdBuffer = encoder.finish();
		encoder.release();
		GraphicsContext::GetQueue().submit(1, &cmdBuffer);

		wgpu::BufferMapCallbackInfo callbackInfo{};
		callbackInfo.mode = wgpu::CallbackMode::WaitAnyOnly;
		callbackInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void*, void*) {
			if (status != wgpu::MapAsyncStatus::Success) {
				LOG_ERROR("Readback buffer map callback: status: {0}, message: {1}", (int)status, message);
			}
			};

		wgpu::Future future = readbackBuffer.mapAsync(wgpu::MapMode::Read, 0, sizeof(Engine::UUID), callbackInfo);
		wgpu::FutureWaitInfo waitInfo;
		waitInfo.future = future;
		wgpuInstanceWaitAny(GraphicsContext::GetInstance(), 1, &waitInfo, 100000000); // 100000000ns = 0.01s

		const uint8_t* mapped = static_cast<const uint8_t*>(readbackBuffer.getConstMappedRange(0, sizeof(Engine::UUID)));

		if (!mapped)
		{
			LOG_ERROR("ReadPixel: mapped range is null!");
			readbackBuffer.release();
			return Engine::UUID(0);
		}
		const uint64_t* pixel = reinterpret_cast<const uint64_t*>(mapped);
		Engine::UUID id(*pixel);

		readbackBuffer.release(); //TODO: must this be?

		return id;
	}

	void EntityIdPass::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		wgpu::TextureDescriptor entityID{};
		entityID.label = { "RendererEntityIDTexture", WGPU_STRLEN };
		entityID.dimension = wgpu::TextureDimension::_2D;
		entityID.format = wgpu::TextureFormat::RG32Uint;	// 64bits for uuid (2x32)
		entityID.size = { width, height, 1 };
		entityID.mipLevelCount = 1;
		entityID.sampleCount = 1;
		entityID.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		m_Texture = GraphicsContext::GetDevice().createTexture(entityID);

		wgpu::TextureViewDescriptor view{};
		view.label = { "RendererEntityIDTextureView", WGPU_STRLEN };
		view.dimension = wgpu::TextureViewDimension::_2D;
		view.format = entityID.format;
		view.baseMipLevel = 0;
		view.mipLevelCount = 1;
		view.baseArrayLayer = 0;
		view.arrayLayerCount = 1;
		m_TextureView = m_Texture.createView(view);
	}

	void EntityIdPass::CreateBindGroupLayout()
	{
		wgpu::BindGroupLayoutEntry entry = wgpu::Default;
		entry.binding = 0;
		entry.visibility = wgpu::ShaderStage::Fragment;
		entry.buffer.type = wgpu::BufferBindingType::Uniform;
		entry.buffer.minBindingSize = sizeof(Engine::UUID);
		entry.buffer.hasDynamicOffset = true;

		wgpu::BindGroupLayoutDescriptor desc{};
		desc.label = { "EntityIdBindGroupLayout", WGPU_STRLEN };
		desc.entryCount = 1;
		desc.entries = &entry;

		m_BindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(desc);

		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "EntityIdUniformBuffer", WGPU_STRLEN };
		bufferDesc.size = 1024 * 256 * sizeof(Engine::UUID); //1024 max entities, 256 bytes alignment
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

		m_UniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);

		wgpu::BindGroupEntry bindGroupEntry{};
		bindGroupEntry.binding = 0;
		bindGroupEntry.buffer = m_UniformBuffer;
		bindGroupEntry.offset = 0;
		bindGroupEntry.size = sizeof(Engine::UUID);

		wgpu::BindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.label = { "EntityIdBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = m_BindGroupLayout;
		bindGroupDesc.entryCount = 1;
		bindGroupDesc.entries = &bindGroupEntry;
		m_BindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

	void EntityIdPass::CreatePipeline()
	{
		std::string content;
		if (!Engine::ReadFile("Resources/Engine/shaders/entityId.wgsl", content))
		{
			throw std::runtime_error("Failed to load entity id shader");
		}

		wgpu::ShaderSourceWGSL shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderCodeDesc.code = { content.c_str(), WGPU_STRLEN };

		wgpu::ShaderModuleDescriptor shaderDesc{};
		shaderDesc.nextInChain = &shaderCodeDesc.chain;

		wgpu::ShaderModule shaderModule = GraphicsContext::GetDevice().createShaderModule(shaderDesc);
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
		colorTarget.format = wgpu::TextureFormat::RG32Uint;
		//colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::DepthStencilState depthStencil{};
		depthStencil.format = wgpu::TextureFormat::Depth24Plus;
		depthStencil.depthWriteEnabled = wgpu::OptionalBool::False;
		depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;
		depthStencil.stencilFront = {};
		depthStencil.stencilBack = {};
		depthStencil.stencilReadMask = 0;
		depthStencil.stencilWriteMask = 0;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.depthStencil = &depthStencil;
		pipelineDesc.fragment = &fragmentState;

		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u; // Default value for the mask, meaning "all bits on"
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			RenderGlobals::GetFrameLayout(),
			RenderGlobals::GetModelLayout(),
			m_BindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "EntityIdShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = 3;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_EntityIdPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
	}
}