#include "EditorApp.h"
#define WEBGPU_CPP_IMPLEMENTATION
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Core/IconManager.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Utility/FileWatcher.h"
#include "Engine/Core/Time.h"
#include "Engine/Utility/File.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/GameContext.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Script/Script.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Utility/Path.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Core/EditorRuntime.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Debug/RenderDoc.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Core/Project.h"
#include "Editor/Core/EditorSettings.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"

namespace Editor
{
	EditorApp::EditorApp(const ApplicationCommandLineArgs& args)
	{
		//RenderDoc::Initialize(); //TODO: enable/disable at runtime in menu bar

		LoadEditorSettings();

		if(args.Count > 1)
		{
			g_EditorSettings.projectDirectory = args[1];
		}
		else
		{
			if(g_EditorSettings.projectDirectory.empty())
			{
				g_EditorSettings.projectDirectory = Engine::OpenDirectory();
			}
		}

		auto project = Engine::Project::Load(g_EditorSettings.projectDirectory);
		SaveEditorSettings();

		std::filesystem::current_path(GetExecutableDir());

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();
		Engine::SamplerPool::Initialize();
		Engine::RenderPipelineLayouts::Initialize();
		Engine::AssetManager::Initialize(project->GetProjectDirectory());

		m_Window.emplace(Engine::WindowSpecification{ "Gerbil Editor", 1600, 900, "Resources/Engine/icons/logo.png" });
		m_Window->SetEventCallback([](Engine::Event& e) {Engine::EventBus::Get().Publish(e); });
		Engine::Input::SetActiveWindow(*static_cast<GLFWwindow*>(m_Window->GetNativeWindow()));

		Engine::g_Renderer.Initialize();

		Engine::Time::Initialize();

		EditorCommandManager::Initialize();
		FileWatcher::WatchDirectory(project->GetAssetsDirectory());

		Engine::Audio::Initialize();
		IconManager::Load("Resources/Editor/icons/icons.png");

		std::filesystem::path dllPath = project->GetProjectDirectory() / "bin/windows/" / BUILD_CONFIG / (project->GetTitle() + ".dll");
		Engine::ScriptRegistry& registry = Engine::ScriptRegistry::Get();

		EditorRuntime::Initialize();
		EditorRuntime::LoadScripts(registry, dllPath);

		Engine::SceneSerializer::Initialize(registry); //TODO: dont pass registry like this...

		EditorWindowManager::Initialize(*m_Window);

		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(project->GetStartSceneID());
		if(&scene) { Engine::SceneManager::SetActiveScene(&scene); }

		Engine::EventBus::Get().Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); });
	}

	EditorApp::~EditorApp()
	{
		FileWatcher::Shutdown();
		Engine::Audio::Shutdown();
		EditorWindowManager::Shutdown();
		IconManager::Unload();
		Engine::AssetManager::Shutdown();
		Engine::SamplerPool::Shutdown();
		m_Window.reset();
		GLFW::Shutdown();
		Engine::GraphicsContext::Shutdown();
	}

	void EditorApp::Run()
	{
		while (m_Running)
		{
			Engine::Time::BeginFrame();				// update delta time and FPS counters
			Engine::Input::Update();					// poll input events
			Engine::Audio::Update();					// release finished audio voices back to pool

			if (m_Window->IsMinimized()) continue;

			EditorWindowManager::Update();		// update editor UI, render viewport, ...
			EditorCommandManager::Flush();		// execute queued commands (deffered execution)
			EditorRuntime::Update();					// update runtime (scripts, audio listener, etc...)
		}
	}
}