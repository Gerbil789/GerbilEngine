#ifndef DIST
#define WEBGPU_CPP_IMPLEMENTATION //TODO: handle macros in premake
#endif

#include "EditorApp.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Core/IconManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Utility/FileWatcher.h"

#include "Engine/Core/Log.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Configuration.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/TransformSystem.h" //TODO: i dont like this system
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Debug/RenderDoc.h"

namespace Editor
{
	namespace
	{
		Engine::Window m_Window;
		bool m_Running = true;
	}

	EditorApp::EditorApp()
	{
		//RenderDoc::Initialize(); //TODO: enable/disable at runtime in menu bar
		EditorSettings::Load();
		Engine::Project::Load(EditorSettings::projectDirectory);
		const Engine::Project& project = Engine::Project::GetActive();

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window.Initialize({ std::format("Gerbil Editor - {}", Engine::Configuration) , 1600, 900, "Resources/Engine/icons/logo.png" });
		
		m_Window.SetEventCallback([](auto& e) {Engine::EventBus::Publish(e); });

		Engine::AssetManager::Initialize(project.GetProjectDirectory());

		Engine::Input::SetActiveWindow(*m_Window.GetNativeWindow());
		EditorContext::renderer.Initialize();
		EditorContext::renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque/* | Engine::RenderPassType::Normal | Engine::RenderPassType::Wireframe*/);
		EditorCommandManager::Initialize();
		FileWatcher::WatchDirectory(project.GetAssetsDirectory());
		Engine::Audio::Initialize();
		IconManager::Initialize();
		EditorWindowManager::Initialize(m_Window);

		Engine::EventBus::Subscribe<Engine::SceneChangedEvent>([this](auto& e) 
			{
				const auto& registry = Engine::AssetManager::GetAssetRegistry();
				m_Window.SetTitle(std::format("Gerbil Editor - {} - Scene: {}", Engine::Configuration, registry.GetRecord(e.id).GetName()));
				return false;
			});

		std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / Engine::Configuration / (project.GetTitle() + ".dll");
		Engine::Runtime::LoadScripts(dllPath);

		Engine::Uuid id = project.GetDefaultSceneId();

		Engine::SceneManager::SetActiveScene(id);

		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(id);
		EditorCommandManager::SetContext(&scene);

		EditorContext::editorCamera.SetBackground(Engine::Camera::Background::Skybox);
		EditorContext::editorCamera.SetPosition(glm::vec3(0.0f, 0.0f, -20.0f));

		static auto applicationCloseListener = Engine::EventBus::Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); return false; });
		LOG_INFO("--- Editor initialization complete ---");
	}

	EditorApp::~EditorApp()
	{
		FileWatcher::Shutdown();
		Engine::Audio::Shutdown();
		EditorWindowManager::Shutdown();
		m_Window.Shutdown();
		GLFW::Shutdown();
		Engine::GraphicsContext::Shutdown();
	}

	void EditorApp::Run()
	{
		while (m_Running)
		{
			if (m_Window.IsMinimized())
			{
				GLFW::WaitEvents();
				Engine::Time::BeginFrame();
				continue;
			}

			Engine::Time::BeginFrame();				// update delta time and FPS counters
			Engine::Input::Update();					// poll input events
			Engine::Audio::Update();					// release finished audio voices back to pool
			Engine::TransformSystem::Update();

			EditorWindowManager::Update();		// update editor UI, render viewport, ...
			EditorCommandManager::ExecuteDefferedCommands();		// execute queued commands (deffered execution)
			
			if (EditorContext::state == EditorState::Play)
			{
				Engine::PhysicsSystem::Update();
				Engine::Runtime::Update();			// update game runtime (scripts, audio listener, etc...)
			}
		}
	}
}