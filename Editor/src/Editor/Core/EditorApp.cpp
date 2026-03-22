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
#include "Engine/Core/Engine.h"
#include "Engine/Core/GameContext.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Script/Script.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Utility/Path.h"
#include "Engine/Core/EngineContext.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Core/EditorRuntime.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Utility/RenderDoc.h"

namespace Editor
{
	bool firstFrame = true;

	EditorApp::EditorApp(const ApplicationCommandLineArgs& args)
	{
		RenderDoc::Initialize();

		std::filesystem::path projectPath = (args.Count > 1) ? std::filesystem::path(args[1]) : Engine::OpenDirectory();
		EditorSelection::SetProject(Project::Load(projectPath));  //TODO: why editor selection owns project????
		std::filesystem::current_path(GetExecutableDir());

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window.emplace(Engine::WindowSpecification{ "Gerbil Editor", 1600, 900, "Resources/Engine/icons/logo.png" });
		m_Window->SetEventCallback([](Engine::Event& e) {Engine::EventBus::Get().Publish(e); });

		Engine::Input::SetActiveWindow(*static_cast<GLFWwindow*>(m_Window->GetNativeWindow()));

		Engine::SamplerPool::Initialize();
		Engine::RenderGlobals::Initialize();
		Engine::Time::Initialize();

		EditorCommandManager::Initialize();
		FileWatcher::WatchDirectory(EditorSelection::GetProject().GetAssetsDirectory());

		Engine::EngineContext context;
		context.ProjectDirectory = EditorSelection::GetProject().GetProjectDirectory();
		context.AssetsDirectory = EditorSelection::GetProject().GetAssetsDirectory();
		Engine::InitializeEngine(context); //TODO: improve engine context

		Engine::AssetManager::Initialize();
		Engine::Audio::Initialize();
		IconManager::Load("Resources/Editor/icons/icons.png");

		const Project& project = EditorSelection::GetProject();
		std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / BUILD_CONFIG / (project.GetTitle() + ".dll");
		Engine::ScriptRegistry& registry = Engine::ScriptRegistry::Get();

		EditorRuntime::Initialize();
		EditorRuntime::LoadScripts(registry, dllPath);

		Engine::SceneSerializer::Initialize(registry); //TODO: dont pass registry like this...
		Engine::SceneManager::Initialize(registry);

		EditorWindowManager::Initialize(*m_Window);

		if (auto id = EditorSelection::GetProject().GetStartSceneID(); id)
		{
			Engine::Scene* scene = Engine::AssetManager::GetAsset<Engine::Scene>(id);
			Engine::SceneManager::SetActiveScene(scene);
		}

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
		RenderDoc::StartFrameCapture();

		while (m_Running)
		{
			Engine::Time::BeginFrame();
			Engine::Input::Update();					// poll input events
			Engine::Audio::Update();					// release finished audio voices back to pool

			if (m_Window->IsMinimized()) continue;

			EditorWindowManager::Update();		// update editor UI, render viewport, ...
			EditorCommandManager::Flush();		// execute queued commands
			EditorRuntime::Update();					// update runtime (scripts, audio listener, etc...)

			if(firstFrame)
			{
				firstFrame = false;
				RenderDoc::EndFrameCapture();
			}
		}
	}
}