#include "EditorLayer.h"
#include "EditorApp.h"

namespace Editor
{
	EditorApp::EditorApp(std::filesystem::path projectPath) : Application("Gerbil Editor - " + projectPath.lexically_normal().filename().string())
	{
		m_Project = Project(projectPath);

		LOG_INFO("Starting Gerbil Editor for project: {0}", m_Project.GetTitle());
		LOG_INFO("Project path: {0}", m_Project.GetPath().string());

		PushLayer(new EditorLayer());
		LOG_INFO("--- Initialization complete ---");
	}

	void EditorApp::Run()
	{
		using namespace Engine;

		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");

			auto now = std::chrono::steady_clock::now();
			float deltaTime = std::chrono::duration<float>(now - m_LastFrameTime).count();
			m_LastFrameTime = now;
			m_FPSCounter.Update(deltaTime);

			Timestep ts(deltaTime); 

			if (!m_Minimized)
			{
				{
					ENGINE_PROFILE_SCOPE("LayerStack OnUpdate");
					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(ts);
					}
				}
			}

			m_Window->OnUpdate();
		}
	}
}