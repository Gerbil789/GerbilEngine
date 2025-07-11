#include "enginepch.h"
#include "Renderer.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/RenderUtils.h"

namespace Engine
{
	Renderer::Renderer()
	{
		ENGINE_PROFILE_FUNCTION();
		SceneManager::RegisterObserver(this);
		m_Device = Application::Get().GetGraphicsContext()->GetDevice();
		m_Queue = Application::Get().GetGraphicsContext()->GetQueue();
		RenderUtils::Initialize(m_Device);
		Resize(m_Width, m_Height);
	}

	Renderer::~Renderer()
	{
		ENGINE_PROFILE_FUNCTION();
		SceneManager::UnregisterObserver(this);
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();

		m_Width = width;
		m_Height = height;

		wgpu::TextureDescriptor desc{};
		desc.label = { "RendererTexture", WGPU_STRLEN };
		desc.dimension = WGPUTextureDimension_2D;
		desc.format = WGPUTextureFormat_RGBA8Unorm;
		desc.size = { width, height, 1 };
		desc.mipLevelCount = 1;
		desc.sampleCount = 1;
		desc.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
		wgpu::Texture texture = m_Device.createTexture(desc);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.label = { "RendererTextureView", WGPU_STRLEN };
		viewDesc.dimension = WGPUTextureViewDimension_2D;
		viewDesc.format = desc.format;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		m_OutputView = texture.createView(viewDesc);
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		ENGINE_PROFILE_FUNCTION();

		wgpu::RenderPassColorAttachment colorAttachment{};
		colorAttachment.view = m_OutputView;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = WGPULoadOp_Clear;
		colorAttachment.storeOp = WGPUStoreOp_Store;
		colorAttachment.clearValue = m_ClearColor;

		wgpu::RenderPassDescriptor renderPassDesc{};
		renderPassDesc.label = { "RenderPassDescriptor", WGPU_STRLEN };
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &colorAttachment;

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
			//auto mesh = entity.GetComponent<MeshComponent>().Mesh;
			//auto material = entity.GetComponent<MeshComponent>().Material;
			auto mesh = AssetManager::GetAsset<Mesh>("resources/models/cube.glb");
			auto material = AssetManager::GetAsset<Material>("resources/materials/default.mat");
			auto shader = material->GetShader();

			auto modelUniformBuffer = entity.GetComponent<MeshComponent>().ModelBuffer;
			auto modelBindGroup = entity.GetComponent<MeshComponent>().ModelBindGroup;
			m_Queue.writeBuffer(modelUniformBuffer, 0, &modelMatrix, sizeof(modelMatrix));


			m_RenderPass.setPipeline(shader->GetRenderPipeline());
			m_RenderPass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
			m_RenderPass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, mesh->GetIndexBuffer().getSize());

			m_RenderPass.setBindGroup(0, modelBindGroup, 0, nullptr);

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

	//Renderer::RendererStatistics Renderer::GetStats()
	//{
	//	return s_Stats;
	//}

	//void Renderer::ResetStats()
	//{
	//	memset(&s_Stats, 0, sizeof(RendererStatistics));
	//}

	//void Renderer::AlignOffset(size_t& currentOffset, size_t alignment)
	//{
	//	size_t padding = currentOffset % alignment;
	//	if (padding != 0)
	//	{
	//		currentOffset += (alignment - padding);
	//	}
	//}
}