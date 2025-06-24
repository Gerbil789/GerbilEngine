#include "EditorLayer.h"
#include "EditorApp.h"

namespace Editor
{
	EditorApp::EditorApp()
	{
		m_EditorLayer = new EditorLayer();
		PushLayer(m_EditorLayer);
		PushLayer(m_EditorLayer->m_ImGuiLayer);

		LOG_INFO("--- Initialization complete ---");
	}

	EditorApp::~EditorApp() {}

	void EditorApp::Run()
	{
		using namespace Engine;

		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");

			auto now = std::chrono::steady_clock::now();
			Timestep timestep = std::chrono::duration<float>(now - m_LastFrameTime).count();
			m_LastFrameTime = now;

			float currentFPS = (timestep > 0.0f) ? 1.0f / timestep : 0.0f;

			// Update FPS history
			m_FPSHistorySum -= m_FPSHistory[m_FPSHistoryIndex]; // Subtract the oldest FPS value
			m_FPSHistory[m_FPSHistoryIndex] = currentFPS;        // Insert the new FPS value
			m_FPSHistorySum += currentFPS;                       // Add the new FPS value to the sum

			m_FPSHistoryIndex = (m_FPSHistoryIndex + 1) % FrameHistorySize; // Update the index

			// Calculate the average FPS over the last 30 frames
			m_AverageFPS = m_FPSHistorySum / FrameHistorySize;

			if (!m_Minimized)
			{
				{
					ENGINE_PROFILE_SCOPE("LayerStack OnUpdate");
					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(timestep);
					}
				}
			}

			{
				ENGINE_PROFILE_SCOPE("ImGuiLayer OnUpdate");
				m_EditorLayer->OnImGuiRender();
			}

			m_Window->OnUpdate();
		}
	}


}
