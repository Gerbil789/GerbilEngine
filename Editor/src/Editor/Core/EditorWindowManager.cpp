#include "EditorWindowManager.h"

namespace Editor
{
	EditorWindowManager::EditorWindowManager()
	{
		m_ContentBrowserWindow = RegisterWindow<ContentBrowserWindow>();
		m_InspectorWindow = RegisterWindow<InspectorWindow>();
		m_SceneHierarchyWindow = RegisterWindow<SceneHierarchyWindow>();
		m_MaterialWindow = RegisterWindow<MaterialWindow>();
		// m_SettingsWindow = RegisterWindow<SettingsWindow>();
		m_StatisticsWindow = RegisterWindow<StatisticsWindow>();
		m_MeshImportWindow = RegisterWindow<MeshImportWindow>();
		m_ViewportWindow = RegisterWindow<ViewportWindow>();
		m_GameWindow = RegisterWindow<GameWindow>();
	}

	void EditorWindowManager::OnUpdate(Engine::Timestep ts)
	{
		m_MenuBar.OnUpdate();
		for (auto& window : m_Windows)
		{
			window->OnUpdate(ts);
		}

		//bool show_demo_window = true;
		//ImGui::ShowDemoWindow(&show_demo_window);
	}

	void EditorWindowManager::OnEvent(Engine::Event& e)
	{
		for (auto& window : m_Windows)
		{
			window->OnEvent(e);
		}
	}
}