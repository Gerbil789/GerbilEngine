#include "enginepch.h"
#include "EditorApp.h"
#include "Editor/Core/EditorSceneController.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Engine/Core/Project.h"

#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Utils/File.h"
//tmp
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetManager.h"

#include "Engine/Asset/Serializer/AssetSerializer.h"

namespace Editor
{
	EditorApp::EditorApp(const Engine::ApplicationSpecification& specification) : Application(specification)
	{
		ENGINE_PROFILE_FUNCTION();

		auto args = specification.args;

		if (args.Count > 1)
		{
			auto projectFilePath = args[1];
			Engine::Project::Load(projectFilePath);
		}
		else
		{
			auto projectDirectoryPath = Engine::OpenDirectory();
			if (projectDirectoryPath.empty())
			{
				LOG_WARNING("Project::New - No path selected for new project.");
				return;
			}

			Engine::Project::Load(projectDirectoryPath);
		}

		EditorWindowManager::Initialize();

		LOG_INFO("--- Editor initialization complete ---");

		Engine::UUID startSceneID = Engine::Project::GetStartSceneID();
		if (!startSceneID.IsValid())
		{
			LOG_WARNING("No valid start scene set in project, creating new scene");
			Engine::SceneManager::CreateScene(Engine::Project::GetAssetsDirectory() / "Scenes/empty.scene");
		}
		else
		{
			auto scene = Engine::AssetManager::GetAsset<Engine::Scene>(startSceneID);
			if (!scene)
			{
				LOG_WARNING("Failed to load start scene with ID: {0}, creating new scene", startSceneID);
				Engine::SceneManager::CreateScene(Engine::Project::GetAssetsDirectory() / "Scenes/empty.scene");
			}
			else
			{
				Engine::SceneManager::SetActiveScene(scene);
			}
		}


		{
			Engine::Scene* scene = Engine::SceneManager::GetActiveScene();
			LOG_INFO("Active scene: {0}", scene->id);

			auto entities = scene->GetEntities();
			LOG_INFO("Scene has {0} entities", entities.size());

			for (auto ent : entities)
			{
				LOG_INFO("Entity: '{0}' with ID: {1}", ent.GetName(), ent.GetUUID());
			}
		}

		Engine::Scene* scene = Engine::SceneManager::GetActiveScene();

		//auto shader = Engine::AssetManager::GetAsset<Engine::Shader>(Engine::UUID(11488752734390452845));

		auto material = Engine::AssetManager::GetAsset<Engine::Material>(Engine::UUID(2306162455903992554));
		//auto material = Engine::AssetManager::CreateAsset<Engine::Material>(Engine::Project::GetAssetsDirectory() / "Materials/Test.material", shader);
		//material->SetVec4("color", glm::vec4(0.8f, 0.1f, 0.2f, 1.0f));

		//Ref<Engine::Texture2D> texture = Engine::TextureImporter::LoadTexture2D("Resources/Editor/icons/skull.png");
		//auto smileTexture = Engine::AssetManager::GetAsset<Engine::Texture2D>(Engine::UUID(16704251879842279232));
		//material->SetTexture("albedoTexture", smileTexture);

		//save material, TODO: make better save function
		// metadata = Engine::AssetManager::GetAssetMetadata(material->id);
		//Engine::AssetSerializer::SerializeAsset(material, *metadata);

		// Entity 1
		{
			auto cube = scene->CreateEntity("FirstCube");
			auto mesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/cube.glb");

			auto& component = cube.AddComponent<Engine::MeshComponent>();
			component.Material = material;
			component.Mesh = mesh;

			cube.GetComponent<Engine::TransformComponent>().Position = { 2.0f, 0.0f, 0.0f };
			cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

			LOG_INFO("Created entity '{0}' with ID: {1}", cube.GetName(), cube.GetUUID());

			EditorSceneController::SelectEntity(cube);
		}


		// Entity 2
		{
			auto cube = scene->CreateEntity("SecondCube");
			auto mesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/cube.glb");

			auto& component = cube.AddComponent<Engine::MeshComponent>();
			component.Material = material;
			component.Mesh = mesh;

			cube.GetComponent<Engine::TransformComponent>().Position = { -2.0f, 0.0f, 0.0f };
			cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

			LOG_INFO("Created entity '{0}' with ID: {1}", cube.GetName(), cube.GetUUID());
		}
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