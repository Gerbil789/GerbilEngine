#include "EditorApp.h"
#define WEBGPU_CPP_IMPLEMENTATION
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Core/IconManager.h"
#include "Editor/Core/EditorSelection.h"
#include "Editor/Utility/FileWatcher.h"
#include "Engine/Core/Time.h"
#include "Engine/Utility/File.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/GameContext.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Script/Script.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Utility/Path.h"
#include "Engine/Core/EngineContext.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Core/EditorRuntime.h"
#include "Engine/Scene/SceneManager.h"

namespace Editor
{
	namespace
	{
		FileWatcher* m_FileWatcher = nullptr;
	}

	EditorApp::EditorApp(const ApplicationCommandLineArgs& args)
	{
		std::filesystem::path projectPath = (args.Count > 1) ? std::filesystem::path(args[1]) : Engine::OpenDirectory();
		EditorSelection::SetProject(Project::Load(projectPath));

		std::filesystem::current_path(GetExecutableDir()); // Set working directory

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window = new Engine::Window({"Gerbil Editor", 1600, 900, "Resources/Engine/icons/logo.png"});
		m_Window->SetEventCallback([this](Engine::Event& e) {this->OnEvent(e); });

		Engine::Input::Initialize(*m_Window);
		Engine::SamplerPool::Initialize();
		Engine::RenderGlobals::Initialize();

		Engine::Time::Initialize();

		Engine::EngineContext context;
		context.ProjectDirectory = EditorSelection::GetProject().GetProjectDirectory();
		context.AssetsDirectory = EditorSelection::GetProject().GetAssetsDirectory();
		Engine::InitializeEngine(context); //TODO: improve engine context

		Engine::AssetManager::Initialize();
		Engine::Audio::Initialize();
		IconManager::Load("Resources/Editor/icons/icons.png");
		EditorWindowManager::Initialize(*m_Window);

		m_FileWatcher = new FileWatcher(EditorSelection::GetProject().GetAssetsDirectory(), [this](std::unique_ptr<Engine::FileEvent> e) {PushFileEvent(std::move(e)); });

		const Project& project = EditorSelection::GetProject();
		std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / BUILD_CONFIG / (project.GetTitle() + ".dll");
		Engine::ScriptRegistry& registry = Engine::ScriptRegistry::Get();

		EditorRuntime::Initialize();
		EditorRuntime::LoadScripts(registry, dllPath);

		Engine::SceneManager::Initialize(registry);

		
		if (auto id = EditorSelection::GetProject().GetStartSceneID(); id)
		{
			Engine::Scene* scene = Engine::AssetManager::GetAsset<Engine::Scene>(id);
			Engine::SceneManager::SetActiveScene(scene);
			//EditorContext::Get().editorScene = scene;
		}

		LOG_INFO("--- Editor initialization complete ---");

		//Engine::Scene* scene = Engine::SceneManager::GetActiveScene();

		//TODO: NEVER HARDCODE ASSETS LIKE THIS, THIS IS JUST FOR TESTING PURPOSES
		//Engine::Material* material = Engine::AssetManager::GetAsset<Engine::Material>(9667627839419811388);
		//Engine::Mesh* mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(9153350241491098746);

		//for (int x = 0; x < 10; x++)
		//{
		//	for (int y = 0; y < 1; y++)
		//	{
		//		for (int z = 0; z < 1; z++)
		//		{
		//			auto cube = scene->CreateEntity("GridCube");
		//			cube.AddComponent<Engine::MeshComponent>(material, mesh);
		//			cube.GetComponent<Engine::TransformComponent>().position = { static_cast<float>(x) * 3.0f - (15), static_cast<float>(y) * 3.0f, static_cast<float>(z) * 3.0f - 30 };
		//		}
		//	}
		//}

		//// camera entity
		//{
		//	auto cameraEntity = scene->CreateEntity("Camera");
		//	auto& component = cameraEntity.AddComponent<Engine::CameraComponent>();
		//	std::unique_ptr<Engine::Camera> camera = std::make_unique<Engine::Camera>();
		//	camera->SetBackground(Engine::Camera::Background::Skybox);
		//	component.camera = camera.release();
		//	scene->SetActiveCamera(cameraEntity);
		//}


		//// player entity
		//{
		//	auto player = scene->CreateEntity("Player");
		//	player.AddComponent<Engine::MeshComponent>(material, mesh);

		//	auto& scriptComponent = player.AddComponent<Engine::ScriptComponent>();
		//	scriptComponent.id = "PlayerController";

		//	Engine::Script* playerScript = registry.GetDescriptor("PlayerController").factory();
		//	playerScript->Self = player;
		//	playerScript->OnCreate();
		//	scriptComponent.instance = std::move(playerScript);
		//}
	}

	EditorApp::~EditorApp()
	{
		ENGINE_PROFILE_FUNCTION();

		delete m_FileWatcher;
		Engine::Audio::Shutdown();
		EditorWindowManager::Shutdown();
		IconManager::Unload();
		Engine::AssetManager::Shutdown();
		Engine::SamplerPool::Shutdown();
		delete m_Window;
		GLFW::Shutdown();
		Engine::GraphicsContext::Shutdown();
	}

	void EditorApp::Run()
	{
		while (m_Running)
		{
			Engine::Time::BeginFrame();
			ProcessFileEvents(); // handle file events from FileWatcher
			m_Window->OnUpdate(); // poll events

			if (m_Window->IsMinimized()) continue;

			EditorWindowManager::OnUpdate(); // update editor UI
			EditorCommandManager::Flush(); // execute queued commands
			
			if (EditorRuntime::GetState() == EditorState::Play)
			{
				EditorRuntime::Update();
			}

		}
	}

	void EditorApp::OnEvent(Engine::Event& e)
	{
		Engine::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<Engine::KeyPressedEvent>([this](auto& e) 
			{
				if(e.GetKey() == Engine::Key::F11)
				{
					m_Window->SetMode((m_Window->GetMode() == Engine::WindowMode::BorderlessFullscreen) ? Engine::WindowMode::Windowed : Engine::WindowMode::BorderlessFullscreen);
				}

				if ((e.GetKey() == Engine::Key::Z || e.GetKey() == Engine::Key::Y) && Engine::Input::IsKeyPressed(Engine::Key::LeftControl))
				{
					(!Engine::Input::IsKeyPressed(Engine::Key::LeftShift) ? EditorCommandManager::Undo() : EditorCommandManager::Redo());
				}
			});

		EditorWindowManager::OnEvent(e);
		Engine::AssetManager::OnEvent(e);
		dispatcher.Dispatch<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); });
	}

	void EditorApp::PushFileEvent(std::unique_ptr<Engine::FileEvent> e)
	{
		std::lock_guard<std::mutex> lock(m_FileEventMutex);
		m_FileEventQueue.push(std::move(e));
	}

	void EditorApp::ProcessFileEvents()
	{
		std::lock_guard<std::mutex> lock(m_FileEventMutex);

		while (!m_FileEventQueue.empty())
		{
			auto& event = m_FileEventQueue.front();
			LOG_INFO("Processing file event: {}", event->GetPath());
			//OnEvent(*event);
			m_FileEventQueue.pop();
		}
	}
}