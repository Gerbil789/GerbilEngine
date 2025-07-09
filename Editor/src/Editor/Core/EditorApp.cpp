#include "enginepch.h"
#include "EditorApp.h"
#include "Editor/Services/EditorServiceRegistry.h"

namespace Editor
{
	EditorApp::EditorApp(std::filesystem::path projectPath) : Application("Gerbil Editor - " + projectPath.lexically_normal().filename().string())
	{
		ENGINE_PROFILE_FUNCTION();
		m_Project = Project(projectPath);

		LOG_TRACE("Starting Gerbil Editor for project: {0}", m_Project.GetTitle());

		m_SceneController = CreateScope<SceneController>();
		EditorServiceRegistry::Register<SceneController>(m_SceneController.get()); //TODO: not sure about this registry approach
		m_EditorWindowManager = CreateScope<EditorWindowManager>();

		Engine::SceneManager::CreateScene("NewScene"); //TODO: load default scene from project if there is one

		LOG_INFO("--- Initialization complete ---");
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

			m_EditorWindowManager->OnUpdate(ts);
			m_Window->OnUpdate();
		}
	}

	void EditorApp::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		Application::OnEvent(e);
		m_SceneController->OnEvent(e);
		m_EditorWindowManager->OnEvent(e); 
	}
}