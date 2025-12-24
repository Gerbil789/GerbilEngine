#pragma once

#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Renderer/BaseRenderPass.h"

class ThumbnailRenderer //TODO: this whole class needs to be reworked
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
		mc.mesh = m_PreviewMesh;

		m_Renderer = new Engine::Renderer(64, 64);
		m_Renderer->AddPass(new Engine::BaseRenderPass());
		m_Renderer->SetCamera(camera);
		m_Renderer->SetScene(scene);
	}

	void SetMaterial(Engine::Material* material)
	{
		m_Entity.GetComponent<Engine::MeshComponent>().material = material;
	}

	wgpu::TextureView Render()
	{
		m_Renderer->RenderScene();

		return m_Renderer->GetTextureView();
	}

private:
	Engine::Renderer* m_Renderer = nullptr;

	Engine::Entity m_Entity;
	Engine::Mesh* m_PreviewMesh = nullptr;
};