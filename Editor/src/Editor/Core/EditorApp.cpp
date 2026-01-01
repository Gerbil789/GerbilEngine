#include "enginepch.h"
#include "EditorApp.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Engine/Core/Project.h"
#include "Editor/Core/EditorIcons.h"
#include "Engine/Core/Time.h"
#include "Engine/Utils/Path.h"
//tmp
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Utils/File.h"
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/AssetSerializer.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/CommandManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Utils/FileWatcher.h"

namespace Editor
{
	Engine::FileWatcher m_FileWatcher;

	EditorApp::EditorApp(const Engine::ApplicationSpecification& specification) : Application(specification)
	{
		ENGINE_PROFILE_FUNCTION();

		if (specification.args.Count > 1)
		{
			auto projectFilePath = specification.args[1];
			Engine::Project::Load(projectFilePath);
		}
		else
		{
			auto projectDirectoryPath = Engine::OpenDirectory(); // Prompt user to select project directory
			if (projectDirectoryPath.empty())
			{
				LOG_WARNING("Project::New - No path selected for new project.");
				return;
			}

			Engine::Project::Load(projectDirectoryPath);
		}

		Engine::AssetManager::Initialize();
		EditorIcons::Load(Engine::TextureImporter::LoadTexture2D("Resources/Editor/icons/icons.png"));


		EditorWindowManager::Initialize();

		m_FileWatcher = Engine::FileWatcher(Engine::Project::GetAssetsDirectory());
		m_FileWatcher.SetEventCallback([this](Engine::Event& e) {this->OnEvent(e); });

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

			auto entities = scene->GetEntities(true);
			LOG_INFO("Scene has {0} entities", entities.size());

			for (auto ent : entities)
			{
				LOG_INFO("Entity: '{0}' with ID: {1}", ent.GetName(), ent.GetUUID());
			}
		}

		Engine::Scene* scene = Engine::SceneManager::GetActiveScene();

		Engine::Material* material = Engine::AssetManager::GetAsset<Engine::Material>(Engine::UUID(2306162455903992554)).get();

		for(int x = 0; x < 10; x++)
		{
			for(int y = 0; y < 1; y++)
			{
				for(int z = 0; z < 1; z++)
				{
					auto cube = scene->CreateEntity("GridCube");
					Engine::Mesh* mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(Engine::UUID(8982589797183355654)).get();
					auto& component = cube.AddComponent<Engine::MeshComponent>();
					component.material = material;
					component.mesh = mesh;
					cube.GetComponent<Engine::TransformComponent>().position = { (float)x * 3.0f - (15), (float)y * 3.0f, (float)z * 3.0f - 30};
				}

			}
		}

		//// Entity 1
		//{
		//	auto cube = scene->CreateEntity("FirstCube");
		//	auto mesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/cube.glb");

		//	auto& component = cube.AddComponent<Engine::MeshComponent>();
		//	component.Material = material;
		//	component.Mesh = mesh;

		//	cube.GetComponent<Engine::TransformComponent>().Position = { 2.0f, 0.0f, 0.0f };
		//	//cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

		//	LOG_INFO("Created entity '{0}' with ID: {1}", cube.GetName(), cube.GetUUID());
		//}


		//// Entity 2
		//{
		//	auto cube = scene->CreateEntity("SecondCube");
		//	auto mesh = Engine::MeshImporter::LoadMesh("Resources/Engine/models/sphere.glb");

		//	auto& component = cube.AddComponent<Engine::MeshComponent>();
		//	component.Material = material;
		//	component.Mesh = mesh;

		//	cube.GetComponent<Engine::TransformComponent>().Position = { 0.0f, 0.0f, 0.0f };
		//	//cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

		//	LOG_INFO("Created entity '{0}' with ID: {1}", cube.GetName(), cube.GetUUID());
		//}

		//// Camera Entity
		//{
		//	auto cam = scene->CreateEntity("camera");

		//	auto& component = cam.AddComponent<Engine::CameraComponent>();
		//	scene->SetActiveCamera(cam);

		//	cam.GetComponent<Engine::TransformComponent>().Position = { 0.0f, 0.0f, 10.0f };

		//	LOG_INFO("Created entity '{0}' with ID: {1}", cam.GetName(), cam.GetUUID());


		//	EditorContext::SelectEntity(cam);
		//}

		//Ref<Engine::AudioClip> clip = Engine::AssetManager::GetAsset<Engine::AudioClip>(Engine::UUID(4188365834911559584));
		//Engine::Audio::Play(clip);
	}

	EditorApp::~EditorApp()
	{
		ENGINE_PROFILE_BEGIN("Shutdown", "profile_shutdown.json");
		ENGINE_PROFILE_FUNCTION();
		Engine::Audio::StopAllSounds();
		EditorWindowManager::Shutdown();
		Engine::AssetManager::Shutdown();

		ENGINE_PROFILE_END();
	}

	void EditorApp::Run()
	{
		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");

			Engine::Time::BeginFrame();

			if (m_Minimized) continue;

			EditorWindowManager::OnUpdate();
			m_Window->OnUpdate();
			m_FileWatcher.OnUpdate();
		}
	}

	void EditorApp::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		Application::OnEvent(e);
		CommandManager::OnEvent(e);
		EditorWindowManager::OnEvent(e);


		if(e.GetCategoryFlags() & Engine::EventCategoryFile)
		{
			Engine::AssetManager::OnEvent(e);
		}
	}

	EditorApp CreateApp(Engine::ApplicationCommandLineArgs args)
	{
		Engine::ApplicationSpecification spec;
		spec.title = "Gerbil Editor";
		spec.workingDirectory = GetExecutableDir();
		spec.args = args;

		return EditorApp(spec);
	}
}