#include "ThumbnailRenderer.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Resources.h"

namespace Editor
{
	namespace
	{
		struct ThumbnailData 
		{
			Engine::Scene scene;
			Engine::Entity entity;
			Engine::Camera camera;

			Engine::Renderer renderer;
		};

		ThumbnailData& GetData() 
		{
			static ThumbnailData data;
			return data;
		}

		std::unordered_map<Engine::Uuid, wgpu::TextureView> m_ThumbnailCache;
	}

	void ThumbnailRenderer::Initialize()
	{
		auto& data = GetData();

		data.camera.SetBackground(Engine::Camera::Background::Color);
		data.camera.SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		data.camera.SetAspectRatio(1.0f);
		data.camera.SetPosition({ 0.0f, 0.0f, 3.0f });
		data.camera.SetRotation({ 0.0f, 180.0f, 0.0f });

		data.entity = data.scene.CreateEntity("PreviewEntity");
		auto& mc = data.entity.Add<Engine::MeshComponent>();

		Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(Engine::RESOURCES::MESH::SPHERE);
		mc.mesh = &mesh;

		data.renderer.Initialize();
		data.renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Opaque);
		data.renderer.SetCamera(&data.camera);
	}

	wgpu::TextureView Render(Engine::Material* material)
	{
		auto& data = GetData();
		data.entity.Get<Engine::MeshComponent>().SetMaterial(0, material);

		wgpu::TextureDescriptor desc;
		desc.label = { "ThumbnailTexture", WGPU_STRLEN };
		desc.dimension = wgpu::TextureDimension::_2D;
		desc.format = wgpu::TextureFormat::RGBA8Unorm;
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

		data.renderer.SetColorTarget(thumbnailView);


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
			data.renderer.SetDepthTarget(depthTexture.createView(depthViewDesc));
		}


		data.renderer.RenderScene(&data.scene);

		return thumbnailView;
	}

	wgpu::TextureView ThumbnailRenderer::GetThumbnail(Engine::Uuid id)
	{
		if (m_ThumbnailCache.contains(id))
		{
			return m_ThumbnailCache[id];
		}

		Engine::Material* material = &(Engine::AssetManager::GetAsset<Engine::Material>(id));
		wgpu::TextureView newView = Render(material);
		m_ThumbnailCache[id] = newView;
		return newView;
	}
}