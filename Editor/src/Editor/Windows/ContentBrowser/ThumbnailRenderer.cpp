#include "ThumbnailRenderer.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Resources.h"

namespace Editor
{
	namespace
	{
		Engine::Scene scene;
		entt::entity entity;
		Engine::Camera camera;
		Engine::Renderer renderer;

		std::unordered_map<Engine::Uuid, wgpu::TextureView> m_ThumbnailCache;
	}

	void ThumbnailRenderer::Initialize()
	{
		camera.SetBackground(Engine::Camera::Background::Color);
		camera.SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		camera.SetAspectRatio(1.0f);
		camera.SetPosition({ 0.0f, 0.0f, 3.0f });
		camera.SetRotation({ 0.0f, 180.0f, 0.0f });

		entity = scene.CreateEntity("PreviewEntity");
		entt::registry& registry = scene.GetRegistry();
		auto& mc = registry.emplace<Engine::MeshComponent>(entity);
		mc.meshId = RESOURCES::MESH::SPHERE;

		renderer.Initialize();
		renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Opaque);
		renderer.SetCamera(&camera);
	}

	wgpu::TextureView Render(Engine::Uuid id)
	{
		entt::registry& registry = scene.GetRegistry();
		registry.get<Engine::MeshComponent>(entity).materials[0] = id;

		wgpu::TextureDescriptor desc;
		desc.label = { "ThumbnailTexture", WGPU_STRLEN };
		desc.dimension = wgpu::TextureDimension::_2D;
		desc.format = Engine::GraphicsContext::GetPreferredSwapChainFormat();
		desc.size = { 64, 64, 1 };
		desc.mipLevelCount = 1;
		desc.sampleCount = 1;
		desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;

		wgpu::Texture thumbnailTexture = Engine::GraphicsContext::GetDevice().createTexture(desc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		viewDesc.format = desc.format;
		viewDesc.arrayLayerCount = 1;
		viewDesc.mipLevelCount = 1;
		wgpu::TextureView thumbnailView = thumbnailTexture.createView(viewDesc);

		renderer.SetColorTarget(thumbnailView);

		//set depth target
		{
			wgpu::TextureDescriptor depthDesc;
			depthDesc.label = { "ThumbnailDepthTexture", WGPU_STRLEN };
			depthDesc.dimension = wgpu::TextureDimension::_2D;
			depthDesc.format = wgpu::TextureFormat::Depth24Plus;
			depthDesc.mipLevelCount = 1;
			depthDesc.sampleCount = 1;
			depthDesc.size = { 64, 64, 1 };
			depthDesc.usage = wgpu::TextureUsage::RenderAttachment;
			wgpu::Texture depthTexture = Engine::GraphicsContext::GetDevice().createTexture(depthDesc);
			wgpu::TextureViewDescriptor depthViewDesc;
			depthViewDesc.dimension = wgpu::TextureViewDimension::_2D;
			depthViewDesc.format = depthDesc.format;
			depthViewDesc.arrayLayerCount = 1;
			depthViewDesc.mipLevelCount = 1;
			renderer.SetDepthTarget(depthTexture.createView(depthViewDesc));
		}

		renderer.RenderScene(scene);
		return thumbnailView;
	}

	wgpu::TextureView ThumbnailRenderer::GetThumbnail(Engine::Uuid id)
	{
		if (m_ThumbnailCache.contains(id))
		{
			return m_ThumbnailCache[id];
		}

		wgpu::TextureView newView = Render(id);
		m_ThumbnailCache[id] = newView;
		return newView;
	}
}