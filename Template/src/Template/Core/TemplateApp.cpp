#include "TemplateApp.h"
#define WEBGPU_CPP_IMPLEMENTATION
#include "Engine/Core/Time.h"
#include "Engine/Utility/File.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/GameContext.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Script/Script.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Utility/Path.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Graphics/GraphicsContext.h"
//#include "Engine/Graphics/Texture.h"
#include "Engine/Asset/Importer/TextureImporter.h"

namespace Template
{
	TemplateApp::TemplateApp(const ApplicationCommandLineArgs& args)
	{
		//std::filesystem::path projectPath = (args.Count > 1) ? std::filesystem::path(args[1]) : Engine::OpenDirectory();
		//EditorSelection::SetProject(Project::Load(projectPath));  //TODO: why editor selection owns project????
		std::filesystem::current_path(GetExecutableDir());

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window.emplace(Engine::WindowSpecification{ "Gerbil Editor", 1600, 900, "Resources/Engine/icons/logo.png" });
		m_Window->SetEventCallback([](Engine::Event& e) {Engine::EventBus::Get().Publish(e); });

		Engine::Input::SetActiveWindow(*static_cast<GLFWwindow*>(m_Window->GetNativeWindow()));

		Engine::SamplerPool::Initialize();
		//Engine::RenderGlobals::Initialize();
		Engine::Time::Initialize();


		//Engine::EngineContext context;
		//context.ProjectDirectory = EditorSelection::GetProject().GetProjectDirectory();
		//context.AssetsDirectory = EditorSelection::GetProject().GetAssetsDirectory();
		//Engine::InitializeEngine(context); //TODO: improve engine context

		//Engine::AssetManager::Initialize();
		Engine::Audio::Initialize();

		//const Project& project = EditorSelection::GetProject();
		//std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / BUILD_CONFIG / (project.GetTitle() + ".dll");
		//Engine::ScriptRegistry& registry = Engine::ScriptRegistry::Get();

		//EditorRuntime::Initialize();
		//EditorRuntime::LoadScripts(registry, dllPath);

		//Engine::SceneSerializer::Initialize(registry); //TODO: dont pass registry like this...
		//Engine::SceneManager::Initialize(registry);

		//EditorWindowManager::Initialize(*m_Window);

		//if (auto id = EditorSelection::GetProject().GetStartSceneID(); id)
		//{
		//	Engine::Scene* scene = Engine::AssetManager::GetAsset<Engine::Scene>(id);
		//	Engine::SceneManager::SetActiveScene(scene);
		//}

		Engine::EventBus::Get().Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); });
	}

	TemplateApp::~TemplateApp()
	{
		Engine::Audio::Shutdown();
		//Engine::AssetManager::Shutdown();
		Engine::SamplerPool::Shutdown();
		m_Window.reset();
		GLFW::Shutdown();
		Engine::GraphicsContext::Shutdown();
	}

	void TemplateApp::Run()
	{
		while (m_Running)
		{
			Engine::Time::BeginFrame();				// update delta time and FPS counters
			Engine::Input::Update();					// poll input events
			Engine::Audio::Update();					// release finished audio voices back to pool

			if (m_Window->IsMinimized()) continue;

			//EditorWindowManager::Update();		// update editor UI, render viewport, ...
			//EditorCommandManager::Flush();		// execute queued commands (deffered execution)
			//EditorRuntime::Update();					// update runtime (scripts, audio listener, etc...)
		}
	}
}