#include "EditorApp.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Engine/Core/Project.h"
#include "Editor/Core/EditorIcons.h"
#include "Engine/Core/Time.h"
//tmp
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Utils/File.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/AssetSerializer.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/CommandManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Utils/FileWatcher.h"
#include "Engine/Core/KeyCodes.h"

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
			Engine::Scene* scene = Engine::AssetManager::GetAsset<Engine::Scene>(startSceneID);
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

		Engine::Material* material = Engine::AssetManager::GetAsset<Engine::Material>(Engine::UUID(2306162455903992554));
		Engine::Mesh* mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(Engine::UUID(8982589797183355654));

		// Create grid of cubes
		for(int x = 0; x < 10; x++)
		{
			for(int y = 0; y < 1; y++)
			{
				for(int z = 0; z < 1; z++)
				{
					auto cube = scene->CreateEntity("GridCube");
					auto& component = cube.AddComponent<Engine::MeshComponent>();
					component.material = material;
					component.mesh = mesh;
					cube.GetComponent<Engine::TransformComponent>().position = { (float)x * 3.0f - (15), (float)y * 3.0f, (float)z * 3.0f - 30};
				}

			}
		}

		// Camera entity
		{
			auto cameraEntity = scene->CreateEntity("Camera");
			auto& component = cameraEntity.AddComponent<Engine::CameraComponent>();
			std::unique_ptr<Engine::Camera> camera = std::make_unique<Engine::Camera>();

			camera->SetBackground(Engine::Camera::Background::Skybox);
			component.camera = camera.release();

			cameraEntity.GetComponent<Engine::TransformComponent>().position = { 0.0f, 0.0f, 0.0f };
			cameraEntity.GetComponent<Engine::TransformComponent>().rotation = { 0.0f, 0.0f, 0.0f };
			scene->SetActiveCamera(cameraEntity);
		}
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
			m_MainWindow->OnUpdate();
			m_FileWatcher.OnUpdate();

			if(m_GameInstance)
			{
				m_GameInstance->Update();
			}
		}
	}

	void EditorApp::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		if (e.GetEventType() == Engine::EventType::KeyPressed)
		{
			Engine::KeyEvent& keyEvent = static_cast<Engine::KeyEvent&>(e);
			if (keyEvent.GetKey() == Engine::Key::F11)
			{
				if (m_MainWindow->GetMode() == Engine::WindowMode::BorderlessFullscreen)
				{
					m_MainWindow->SetMode(Engine::WindowMode::Windowed);
				}
				else
				{
					m_MainWindow->SetMode(Engine::WindowMode::BorderlessFullscreen);
				}
			}
		}

		Application::OnEvent(e);
		CommandManager::OnEvent(e);
		EditorWindowManager::OnEvent(e);

		if(e.GetCategoryFlags() & Engine::EventCategoryFile)
		{
			Engine::AssetManager::OnEvent(e);
		}
	}

	void EditorApp::PlayGame()
	{
		if (m_GameInstance) return; // already playing

		m_GameInstance = std::make_unique<GameInstance>();
		m_GameInstance->OnExit = [this]() { this->m_GameInstance.reset(); };
		Engine::Scene* activeScene = Engine::SceneManager::GetActiveScene();
		m_GameInstance->Initialize(activeScene);
	}

	EditorApp CreateApp(Engine::ApplicationCommandLineArgs args)
	{
		Engine::ApplicationSpecification spec;
		spec.title = "Gerbil Editor";
		spec.args = args;
		return EditorApp(spec);
	}
}