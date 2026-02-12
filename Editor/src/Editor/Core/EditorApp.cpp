#include "EditorApp.h"
#define WEBGPU_CPP_IMPLEMENTATION
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Core/IconManager.h"
#include "Engine/Core/Time.h"

#include "Engine/Utils/File.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Audio/Audio.h"
//#include "Engine/Utils/FileWatcher.h"
#include "Engine/Core/KeyCodes.h"

#include "Engine/Core/Engine.h"
#include "Engine/Core/API.h"
#include "Engine/Core/GameContext.h"


#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Script/Script.h"
#include "Engine/Event/KeyEvent.h"

#include "Engine/Core/Input.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Utils/Path.h"
#include "Engine/Core/EngineContext.h"
#include "Engine/Event/ApplicationEvent.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Core/Runtime.h"

#include <windows.h>

namespace Editor
{
	//Engine::FileWatcher m_FileWatcher;

	EditorApp::EditorApp(const ApplicationSpecification& specification)
	{
		ENGINE_PROFILE_FUNCTION();

		if (specification.args.Count > 1)
		{
			auto projectFilePath = specification.args[1];
			EditorContext::SetProject(Project::Load(projectFilePath));
		}
		else
		{
			EditorContext::SetProject(Project::Load(Engine::OpenDirectory())); // Prompt user to select project directory
		}

		std::filesystem::current_path(GetExecutableDir()); // Set working directory

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window = new Engine::Window({ specification.title, 1600, 900, "Resources/Engine/icons/logo.png" });
		m_Window->SetEventCallback([this](Engine::Event& e) {this->OnEvent(e); });

		Engine::Input::Initialize(*m_Window);
		Engine::SamplerPool::Initialize();
		Engine::RenderGlobals::Initialize();
		Engine::Audio::Initialize();
		Engine::Time::Initialize();

		Engine::EngineContext context;
		context.ProjectDirectory = EditorContext::GetProject().GetProjectDirectory();
		context.AssetsDirectory = EditorContext::GetProject().GetAssetsDirectory();
		Engine::InitializeEngine(context); //TODO: improve engine context

		Engine::AssetManager::Initialize();
		IconManager::Load("Resources/Editor/icons/icons.png");
		EditorWindowManager::Initialize(*m_Window);

		//m_FileWatcher = Engine::FileWatcher(EditorContext::GetProject().GetAssetsDirectory());
		//m_FileWatcher.SetEventCallback([this](Engine::Event& e) {this->OnEvent(e); });

		LOG_INFO("--- Editor initialization complete ---");

		Engine::Uuid startSceneID = EditorContext::GetProject().GetStartSceneID();
		if (!startSceneID.IsValid())
		{
			LOG_WARNING("No valid start scene set in project, creating new scene");
			Engine::SceneManager::CreateScene(EditorContext::GetProject().GetAssetsDirectory() / "Scenes/empty.scene");
		}
		else
		{
			Engine::Scene* scene = Engine::AssetManager::GetAsset<Engine::Scene>(startSceneID);
			if (!scene)
			{
				LOG_WARNING("Failed to load start scene with ID: {}, creating new scene", startSceneID);
				Engine::SceneManager::CreateScene(EditorContext::GetProject().GetAssetsDirectory() / "Scenes/empty.scene");
			}
			else
			{
				Engine::SceneManager::SetActiveScene(scene);
			}
		}


		{
			Engine::Scene* scene = Engine::SceneManager::GetActiveScene();
			LOG_INFO("Active scene: {}", scene->id);

			auto entities = scene->GetEntities(true);
			LOG_INFO("Scene has {} entities", entities.size());

			for (auto ent : entities)
			{
				LOG_INFO("Entity: '{}' with ID: {}", ent.GetName(), ent.GetUUID());
			}
		}

		Engine::Scene* scene = Engine::SceneManager::GetActiveScene();

		Engine::Material* material = Engine::AssetManager::GetAsset<Engine::Material>(Engine::Uuid(9667627839419811388));
		Engine::Mesh* mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(Engine::Uuid(9153350241491098746));

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


		// DLL test
		std::filesystem::path dllPath = EditorContext::GetProject().GetProjectDirectory() / "bin/windows/Debug/TestProject.dll";

		//check if path is valid
		if (!std::filesystem::exists(dllPath))
		{
			throw std::runtime_error("DLL path does not exist: " + dllPath.string());
		}

		HMODULE gameModule = LoadLibraryA(dllPath.string().c_str());
		if (!gameModule)
		{
			throw std::runtime_error("Failed to load TestProject.dll");
		}

		using GameRegisterScriptsFn = int(*)(Engine::ScriptRegistry&);
		auto Game_Register_Fn = (GameRegisterScriptsFn)GetProcAddress(gameModule, "RegisterScripts");

		if (!Game_Register_Fn)
		{
			throw std::runtime_error("Failed to load RegisterScripts function from TestProject.dll");
		}

		Engine::ScriptRegistry& registry = Engine::ScriptRegistry::Get();
		Game_Register_Fn(registry);

		auto scripts = registry.GetAll();

		LOG_INFO("Total Registered Scripts: {}", scripts.size());
		for (const auto& script : scripts)
		{
			LOG_INFO("Registered Script: {}", script->name);
		}

		auto player = scene->CreateEntity("Player");
		auto& component = player.AddComponent<Engine::MeshComponent>();
		component.material = material;
		component.mesh = mesh;

		auto& scriptComponent = player.AddComponent<Engine::ScriptComponent>();
		scriptComponent.id = "PlayerController";

		Engine::Script* playerScript = registry.Get("PlayerController").factory();
		playerScript->Self = player;
		playerScript->OnCreate();
		scriptComponent.instance = std::move(playerScript);
	}

	EditorApp::~EditorApp()
	{
		ENGINE_PROFILE_BEGIN("Shutdown", "profile_shutdown.json");
		ENGINE_PROFILE_FUNCTION();
		Engine::Audio::StopAllSounds();
		EditorWindowManager::Shutdown();
		IconManager::Unload();
		Engine::AssetManager::Shutdown();


		Engine::Audio::Shutdown();
		Engine::SamplerPool::Shutdown();
		Engine::GraphicsContext::Shutdown();
		delete m_Window;
		GLFW::Shutdown();

		ENGINE_PROFILE_END();
	}

	void EditorApp::Run()
	{
		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");

			Engine::Time::BeginFrame();
			m_Window->OnUpdate(); // poll events

			if (m_Window->IsMinimized()) continue;

			float delta = Engine::Time::DeltaTime();

			//// update scripts, TODO: update this only in runtime
			//for(auto& ent : Engine::SceneManager::GetActiveScene()->GetEntities<Engine::ScriptComponent>())
			//{
			//	auto& scriptComp = ent.GetComponent<Engine::ScriptComponent>();
			//	if (scriptComp.instance)
			//	{
			//		scriptComp.instance->OnUpdate(delta);
			//	}
			//}

			EditorWindowManager::OnUpdate(); // process editor UI

			EditorCommandManager::Flush(); // execute queued commands

			//m_FileWatcher.OnUpdate(); // check for file changes

			Engine::Runtime::Update(delta);
		}
	}

	void EditorApp::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		Engine::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<Engine::KeyPressedEvent>([this](auto& e) 
			{
				if(e.GetKey() == Engine::Key::F11)
				{
					m_Window->SetMode((m_Window->GetMode() == Engine::WindowMode::BorderlessFullscreen) ? Engine::WindowMode::Windowed : Engine::WindowMode::BorderlessFullscreen);
				}
			});

		EditorCommandManager::OnEvent(e);
		EditorWindowManager::OnEvent(e);

		Engine::AssetManager::OnEvent(e);
		dispatcher.Dispatch<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); });
	}
}