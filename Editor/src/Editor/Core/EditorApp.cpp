#include "enginepch.h"
#include "EditorApp.h"
#include "Editor/Core/EditorSceneController.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Engine/Core/Project.h"

#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
//tmp
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Renderer/Shaders/PhongShader.h"
#include "Engine/Renderer/Shaders/FlatColorShader.h"

namespace Editor
{
	EditorApp::EditorApp(const Engine::ApplicationSpecification& specification) : Application(specification)
	{
		ENGINE_PROFILE_FUNCTION();

		auto commandLineArgs = specification.args;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			Engine::Project::Load(projectFilePath);
		}
		else
		{
			Engine::Project::New();
		}

		EditorWindowManager::Initialize();

		Engine::SceneManager::CreateScene("MyScene");

		{
			Engine::Scene* scene = Engine::SceneManager::GetActiveScene();
			LOG_INFO("Active scene: {0}", scene->id);

			Ref<Engine::Texture2D> texture = Engine::TextureImporter::LoadTexture2D("Editor/resources/icons/skull.png");

			// Entity 1
			{
				auto cube = scene->CreateEntity("TextureCube");
				auto mesh = Engine::MeshImporter::LoadMesh("Engine/resources/models/cube.glb");
				//auto material = Engine::MaterialImporter::LoadMaterial("Engine/resources/materials/red.material");
				auto material = Engine::AssetManager::CreateAsset<Engine::Material>("Engine/resources/materials/red.material");
				material->SetTexture("AlbedoTexture", texture);
				material->SetShader(CreateRef<Engine::PhongShader>());
				material->SetValue("Color", glm::vec4(0.8f, 0.1f, 0.2f, 1.0f));

				auto& component = cube.AddComponent<Engine::MeshComponent>();
				component.Material = material;
				component.Mesh = mesh;

				cube.GetComponent<Engine::TransformComponent>().Position = { 2.0f, 0.0f, 0.0f };
				cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

				LOG_WARNING("Created entity '{0}' with ID: {1}", cube.GetName(), cube.GetUUID());

				EditorSceneController::SelectEntity(cube);
			}


			// Entity 2
		/*	{
				auto cube = scene->CreateEntity("BlueCube");
				auto mesh = Engine::AssetManager::Internal::GetAssetByPath<Engine::Mesh>("Engine/resources/models/cube.glb");
				auto material = Engine::AssetManager::Internal::GetAssetByPath<Engine::Material>("Engine/resources/materials/blue.material");
				material->SetShader(CreateRef<Engine::FlatColorShader>());
				material->SetValue("Color", glm::vec4(0.2f, 0.1f, 0.8f, 1.0f));
				auto& component = cube.AddComponent<Engine::MeshComponent>();
				component.Material = material;
				component.Mesh = mesh;

				cube.GetComponent<Engine::TransformComponent>().Position = { -2.0f, 0.0f, 0.0f };
				cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

				LOG_WARNING("Created entity '{0}' with ID: {1}", cube.GetName(), cube.GetUUID());
			}*/
		}

		LOG_INFO("--- Editor initialization complete ---");
	}

	EditorApp::~EditorApp()
	{
		ENGINE_PROFILE_FUNCTION();
		EditorWindowManager::Shutdown();
	}

	void EditorApp::Run()
	{
		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");

			//TODO: pack this fps counter into something, dont bloat main loop with it
			// -----------------------------------
			auto now = std::chrono::steady_clock::now();
			float deltaTime = std::chrono::duration<float>(now - m_LastFrameTime).count();
			m_LastFrameTime = now;
			m_FPSCounter.Update(deltaTime);
			Engine::Timestep ts(deltaTime);
			// -----------------------------------

			if (m_Minimized) continue;

			EditorWindowManager::OnUpdate(ts);
			m_Window->OnUpdate();
		}
	}

	void EditorApp::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		Application::OnEvent(e);
		EditorSceneController::OnEvent(e);
		EditorWindowManager::OnEvent(e);
	}
}