#pragma once

#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Renderer/Camera.h"

class ThumbnailRenderer
{
public:
	void Initialize()
	{
		Engine::Scene* scene = new Engine::Scene();

		Engine::Camera* camera = new Engine::Camera();
		camera->SetBackgroundType(Engine::Camera::BackgroundType::Color);
		camera->SetViewportSize({ 64.0f, 64.0f });
		camera->SetPosition({ 0.0f, 0.0f, 3.0f });
		camera->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

		m_PreviewMesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/sphere.glb");

		m_Entity = scene->CreateEntity("PreviewEntity");
		auto& mc = m_Entity.AddComponent<Engine::MeshComponent>();
		mc.Mesh = m_PreviewMesh;

		m_Renderer = new Engine::Renderer(64, 64);
		m_Renderer->SetCamera(camera);
		m_Renderer->SetScene(scene);
	}

	void SetMaterial(Ref<Engine::Material> material)
	{
		m_Entity.GetComponent<Engine::MeshComponent>().Material = material;
	}

	wgpu::TextureView Render()
	{
		m_Renderer->BeginScene();
		m_Renderer->RenderScene();
		m_Renderer->EndScene();

		return m_Renderer->GetTextureView();
	}

private:
	Engine::Renderer* m_Renderer = nullptr;

	Engine::Entity m_Entity;
	Engine::Mesh* m_PreviewMesh = nullptr;
};