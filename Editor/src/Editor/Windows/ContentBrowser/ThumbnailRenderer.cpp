#include "ThumbnailRenderer.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"

namespace Editor
{
	namespace
	{
		Engine::Entity m_Entity;
		Engine::Scene m_Scene;
		Engine::Camera* m_Camera = nullptr; //TODO: dont heap allocated the camera, but now it must be because skybox initialization inside the camera, fix it when skybox/environment is improved
	}

	void ThumbnailRenderer::Initialize()
	{
		m_Camera = new Engine::Camera();
		m_Camera->SetBackground(Engine::Camera::Background::Color);
		m_Camera->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		m_Camera->SetAspectRatio(1.0f);
		m_Camera->SetPosition({ 0.0f, 0.0f, 3.0f });
		m_Camera->SetRotation({ 0.0f, 180.0f, 0.0f });

		m_Entity = m_Scene.CreateEntity("PreviewEntity");
		auto& mc = m_Entity.Add<Engine::MeshComponent>();
		mc.mesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/sphere.glb");
	}

	wgpu::TextureView ThumbnailRenderer::Render(Engine::Material* material)
	{
		m_Entity.Get<Engine::MeshComponent>().SetMaterial(0, material);

		Engine::Renderer renderer;

		renderer.Resize(64, 64);
		renderer.AddPass(new Engine::BackgroundPass());
		renderer.AddPass(new Engine::OpaquePass());
		renderer.SetCamera(m_Camera);
		renderer.SetScene(&m_Scene);
		renderer.RenderScene();

		return renderer.GetTextureView();
	}
}