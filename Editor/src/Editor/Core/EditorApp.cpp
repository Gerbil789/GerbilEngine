#include "EditorLayer.h"
#include "EditorApp.h"

namespace Editor
{
	EditorApp::EditorApp() : Application("Gerbil Editor")
	{
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