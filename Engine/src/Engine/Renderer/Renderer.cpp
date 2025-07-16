#include "enginepch.h"
#include "Renderer.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/RenderUtils.h"

namespace Engine
{
	Renderer::Renderer()
	{
		ENGINE_PROFILE_FUNCTION();
		m_Scene = SceneManager::GetActiveScene();
		m_Device = GraphicsContext::GetDevice();
		m_Queue = GraphicsContext::GetQueue();
		Resize(640, 640);
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
		wgpu::Texture colorTexture = m_Device.createTexture(colorTextureDesc);

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

	void Renderer::BeginScene(const Camera& camera)
	{
		ENGINE_PROFILE_FUNCTION();

		wgpu::RenderPassColorAttachment colorAttachment{};
		colorAttachment.view = m_TextureView;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		colorAttachment.clearValue = m_ClearColor;

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

		wgpu::RenderPassDescriptor renderPassDesc{};
		renderPassDesc.label = { "RenderPassDescriptor", WGPU_STRLEN };
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &colorAttachment;
		renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = { "RendererCommandEncoderDescriptor", WGPU_STRLEN };
		m_CommandEncoder = m_Device.createCommandEncoder(encoderDesc);

		m_RenderPass = m_CommandEncoder.beginRenderPass(renderPassDesc);

		RenderUtils::FrameUniforms frameUniforms;
		frameUniforms.view = camera.GetViewMatrix();
		frameUniforms.projection = camera.GetProjectionMatrix();
		frameUniforms.cameraPosition = camera.GetPosition();
		wgpu::Buffer frameUniformBuffer = RenderUtils::CreateFrameBuffer();
		wgpu::BindGroup frameBindGroup = RenderUtils::CreateFrameBindGroup(frameUniformBuffer);
		m_Queue.writeBuffer(frameUniformBuffer, 0, &frameUniforms, sizeof(frameUniforms));
		m_RenderPass.setBindGroup(1, frameBindGroup, 0, nullptr);
	}

	void Renderer::RenderScene()
	{
		ENGINE_PROFILE_FUNCTION();

		//TODO: sort entities by material(shader and its pipeline, material and mesh) and render them in batches

		std::vector<Engine::Entity> entities = m_Scene->GetEntities<TransformComponent, MeshComponent>();
		for (auto entity : entities)
		{
			auto meshComponent = entity.GetComponent<MeshComponent>();

			glm::mat4 modelMatrix = entity.GetComponent<TransformComponent>().GetModelMatrix();

			auto mesh = meshComponent.Mesh;
			auto material = meshComponent.Material;
			auto shader = material->GetShader();

			auto modelUniformBuffer = meshComponent.ModelBuffer;
			auto modelBindGroup = meshComponent.ModelBindGroup;

			m_Queue.writeBuffer(modelUniformBuffer, 0, &modelMatrix, sizeof(modelMatrix));
			m_RenderPass.setBindGroup(0, modelBindGroup, 0, nullptr);

			m_RenderPass.setPipeline(shader->GetRenderPipeline());
			m_RenderPass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
			m_RenderPass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, mesh->GetIndexBuffer().getSize());

			m_RenderPass.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);
		}
	}


	void Renderer::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();
		m_RenderPass.end();
		m_RenderPass.release();

		wgpu::CommandBuffer commandBuffer = m_CommandEncoder.finish();
		m_CommandEncoder.release();
		m_Queue.submit(1, &commandBuffer);
	}

}