#include "ThumbnailRenderer.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Asset/AssetManager.h"

namespace Editor
{
	namespace
	{
		struct ThumbnailData 
		{
			Engine::Scene scene;
			Engine::Entity entity;
			Engine::Camera camera;
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
		mc.mesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/sphere.glb");
	}

	wgpu::TextureView Render(Engine::Material* material)
	{
		auto& data = GetData();
		data.entity.Get<Engine::MeshComponent>().SetMaterial(0, material);

		Engine::Renderer renderer;

		renderer.Resize(64, 64);
		renderer.AddPass(new Engine::BackgroundPass());
		renderer.AddPass(new Engine::OpaquePass());
		renderer.SetCamera(&data.camera);
		renderer.SetScene(&data.scene);
		renderer.RenderScene();

		return renderer.GetTextureView();
	}

	wgpu::TextureView ThumbnailRenderer::GetThumbnail(Engine::Uuid id)
	{
		if (m_ThumbnailCache.contains(id))
		{
			return m_ThumbnailCache[id];
		}

		Engine::Material* material = Engine::AssetManager::GetAsset<Engine::Material>(id);
		wgpu::TextureView newView = Render(material);
		m_ThumbnailCache[id] = newView;
		return newView;
	}
}