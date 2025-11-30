#include "enginepch.h"
#include "Renderer.h"
#include "Engine/Core/Application.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/Material.h"

namespace Engine
{
	//TODO: move these structs somewhere...
	struct alignas(16) FrameUniforms {
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 cameraPosition;
		float padding;
	};
	static_assert(sizeof(FrameUniforms) % 16 == 0);


	struct alignas(16) ModelUniforms
	{
		glm::mat4 modelMatrix;
	};
	static_assert(sizeof(ModelUniforms) % 16 == 0);

	uint32_t CeilToNextMultiple(uint32_t value, uint32_t step) 
	{
		uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
		return step * divide_and_ceil;
	}

	void Renderer::Initialize()
	{
		s_Device = GraphicsContext::GetDevice();
		s_Queue = GraphicsContext::GetQueue();

		// Model 
		{
			wgpu::Limits limits;
			s_Device.getLimits(&limits);
			s_ModelUniformStride = CeilToNextMultiple((uint32_t)sizeof(ModelUniforms), (uint32_t)limits.minUniformBufferOffsetAlignment);

			// stride is probably 256 on most hardware, but could be different on some

			const size_t BufferSize = 1024 * s_ModelUniformStride; // max 1024 unique transforms per frame

			wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
			bindGroupLayoutEntry.binding = 0;
			bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			bindGroupLayoutEntry.buffer.hasDynamicOffset = true;
			bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
			bindGroupLayoutEntry.buffer.minBindingSize = sizeof(ModelUniforms);

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
			bindGroupLayoutDesc.label = { "ModelBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = 1;
			bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
			s_ModelBindGroupLayout = s_Device.createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "ModelUniformBuffer", WGPU_STRLEN };;
			bufferDesc.size = BufferSize;
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
			s_ModelUniformBuffer = s_Device.createBuffer(bufferDesc);

			wgpu::BindGroupEntry bindGroupEntry{};
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = s_ModelUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(ModelUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "ModelBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_ModelBindGroupLayout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			s_ModelBindGroup = s_Device.createBindGroup(bindGroupDesc);
		}
		
		// Frame 
		{
			wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
			bindGroupLayoutEntry.binding = 0;
			bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
			bindGroupLayoutEntry.buffer.minBindingSize = sizeof(FrameUniforms);

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
			bindGroupLayoutDesc.label = { "FrameBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = 1;
			bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
			s_FrameBindGroupLayout = s_Device.createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "FrameUniformBuffer", WGPU_STRLEN };;
			bufferDesc.size = sizeof(FrameUniforms);
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

			s_FrameUniformBuffer = s_Device.createBuffer(bufferDesc);

			wgpu::BindGroupEntry bindGroupEntry{};
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = s_FrameUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(FrameUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "FrameBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_FrameBindGroupLayout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			s_FrameBindGroup = s_Device.createBindGroup(bindGroupDesc);
		}
	}

	Renderer::Renderer(uint32_t width, uint32_t height)
	{
		Resize(width, height);
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();

		// Color texture
		wgpu::TextureDescriptor colorTextureDesc{};
		colorTextureDesc.label = { "RendererColorTexture", WGPU_STRLEN };
		colorTextureDesc.dimension = wgpu::TextureDimension::_2D;
		colorTextureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
		colorTextureDesc.size = { width, height, 1 };
		colorTextureDesc.mipLevelCount = 1;
		colorTextureDesc.sampleCount = 1;
		colorTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
		wgpu::Texture colorTexture = s_Device.createTexture(colorTextureDesc);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.label = { "RendererTextureView", WGPU_STRLEN };
		viewDesc.dimension = WGPUTextureViewDimension_2D;
		viewDesc.format = colorTextureDesc.format;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		m_TextureView = colorTexture.createView(viewDesc);

		// Depth texture
		wgpu::TextureDescriptor depthTextureDesc;
		colorTextureDesc.label = { "RendererDepthTexture", WGPU_STRLEN };
		depthTextureDesc.dimension = wgpu::TextureDimension::_2D;
		depthTextureDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthTextureDesc.mipLevelCount = 1;
		depthTextureDesc.sampleCount = 1;
		depthTextureDesc.size = { width, height, 1 };
		depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
		depthTextureDesc.viewFormatCount = 1;
		depthTextureDesc.viewFormats = (WGPUTextureFormat*)&wgpu::TextureFormat::Depth24Plus;
		wgpu::Texture depthTexture = s_Device.createTexture(depthTextureDesc);

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

	void Renderer::BeginScene()
	{
		ENGINE_PROFILE_FUNCTION();
		wgpu::RenderPassColorAttachment colorAttachment{};
		colorAttachment.view = m_TextureView;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		glm::vec4 col = m_Camera->GetClearColor();
		colorAttachment.clearValue = wgpu::Color(col.r, col.g, col.b, col.a);

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
		renderPassDescriptor.label = { "RenderPass", WGPU_STRLEN };
		renderPassDescriptor.colorAttachmentCount = 1;
		renderPassDescriptor.colorAttachments = &colorAttachment;
		renderPassDescriptor.depthStencilAttachment = &depthStencilAttachment;

		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = { "RendererCommandEncoder", WGPU_STRLEN };
		m_CommandEncoder = s_Device.createCommandEncoder(encoderDesc);

		m_RenderPass = m_CommandEncoder.beginRenderPass(renderPassDescriptor);

		FrameUniforms frameUniforms;
		frameUniforms.view = m_Camera->GetViewMatrix();
		frameUniforms.projection = m_Camera->GetProjectionMatrix();
		frameUniforms.cameraPosition = m_Camera->GetPosition();

		s_Queue.writeBuffer(s_FrameUniformBuffer, 0, &frameUniforms, sizeof(frameUniforms));
		m_RenderPass.setBindGroup(GroupID::Frame, s_FrameBindGroup, 0, nullptr);
	}

	void Renderer::RenderScene()
	{
		ENGINE_PROFILE_FUNCTION();

		if(m_Camera->GetBackgroundType() == Camera::BackgroundType::Skybox)
		{
			// Skybox
			auto& skybox = m_Camera->GetSkybox();
			m_RenderPass.setPipeline(skybox.GetShader().GetRenderPipeline());
			m_RenderPass.setBindGroup(1, skybox.GetBindGroup(), 0, nullptr);
			m_RenderPass.draw(36, 1, 0, 0);
		}

		// Entities
		const std::vector<Entity>& entities = m_Scene->GetEntities<TransformComponent, MeshComponent>();
		std::unordered_map<Ref<Material>, std::vector<Entity>> materialGroups;

		// Group entities by material
		for (auto entity : entities)
		{
			auto& meshComponent = entity.GetComponent<MeshComponent>();
			if(meshComponent.Mesh == nullptr)
			{
				continue;
			}

			Ref<Material> material = meshComponent.Material;

			if(material == nullptr)
			{
				material = Material::GetDefault();
			}

			materialGroups[material].push_back(entity);
		}

		uint32_t i = 0;
		for (const auto& [material, groupEntities] : materialGroups)
		{
			Ref<Shader> shader = material->GetShader();
			material->Bind(m_RenderPass);
			m_RenderPass.setPipeline(shader->GetRenderPipeline());

			for (auto entity : groupEntities)
			{
				auto& meshComponent = entity.GetComponent<MeshComponent>();
				auto& mesh = meshComponent.Mesh;

				if (!mesh) continue;

				glm::mat4 modelMatrix = entity.GetComponent<TransformComponent>().GetWorldMatrix(m_Scene->GetRegistry());

				uint32_t dynamicOffset = i * s_ModelUniformStride;
				s_Queue.writeBuffer(s_ModelUniformBuffer, dynamicOffset, &modelMatrix, sizeof(modelMatrix));
				m_RenderPass.setBindGroup(GroupID::Model, s_ModelBindGroup, 1, &dynamicOffset);

				m_RenderPass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				m_RenderPass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, mesh->GetIndexBuffer().getSize());
				m_RenderPass.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);

				i++;
			}
		}
	}

	void Renderer::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();
		m_RenderPass.end();
		m_RenderPass.release();

		wgpu::CommandBuffer commandBuffer = m_CommandEncoder.finish();
		m_CommandEncoder.release();
		s_Queue.submit(1, &commandBuffer);
	}
}