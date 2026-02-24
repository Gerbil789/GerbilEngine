#include "ThumbnailRenderer.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"

namespace Editor
{
	Engine::Renderer* m_Renderer = nullptr;
	Engine::Entity m_Entity;
	Engine::Mesh* m_PreviewMesh = nullptr;

	void ThumbnailRenderer::Initialize()
	{
		Engine::Scene* scene = new Engine::Scene();
		Engine::Camera* camera = new Engine::Camera();
		camera->SetBackground(Engine::Camera::Background::Color);
		constexpr float aspectRatio = 64.0f / 64.0f;
		camera->SetAspectRatio(aspectRatio);
		camera->SetPosition({ 0.0f, 0.0f, 3.0f });
		camera->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

		m_PreviewMesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/sphere.glb");

		m_Entity = scene->CreateEntity("PreviewEntity");
		auto& mc = m_Entity.AddComponent<Engine::MeshComponent>();
		mc.mesh = m_PreviewMesh;

		m_Renderer = new Engine::Renderer;
		m_Renderer->Resize(64, 64);
		m_Renderer->AddPass(new Engine::BackgroundPass());
		m_Renderer->AddPass(new Engine::OpaquePass());
		m_Renderer->SetCamera(camera);
		m_Renderer->SetScene(scene);
	}

	wgpu::TextureView ThumbnailRenderer::Render(Engine::Material* material)
	{
		m_Entity.GetComponent<Engine::MeshComponent>().mesh->SetMaterial(0, material);
		m_Renderer->RenderScene();

		return m_Renderer->GetTextureView();
	}
}