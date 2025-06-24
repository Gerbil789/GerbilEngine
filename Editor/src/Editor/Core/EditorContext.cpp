#include "EditorContext.h"

namespace Editor
{
	EditorContext::EditorContext()
	{
		m_ContentBrowserWindow = new ContentBrowserWindow(this);
		m_InspectorWindow = new InspectorWindow(this);
		m_SceneHierarchyWindow = new SceneHierarchyWindow(this);
		m_MaterialWindow = new MaterialWindow(this);
		m_SettingsWindow = new SettingsWindow(this);
		m_StatisticsWindow = new StatisticsWindow(this);
		m_MeshImportWindow = new MeshImportWindow(this);
		m_ViewportWindow = new ViewportWindow(this);
		m_GameWindow = new GameWindow(this);

		m_Windows.push_back(m_ContentBrowserWindow);
		m_Windows.push_back(m_InspectorWindow);
		m_Windows.push_back(m_SceneHierarchyWindow);
		m_Windows.push_back(m_MaterialWindow);
		m_Windows.push_back(m_SettingsWindow);
		m_Windows.push_back(m_StatisticsWindow);
		m_Windows.push_back(m_MeshImportWindow);
		m_Windows.push_back(m_ViewportWindow);
		m_Windows.push_back(m_GameWindow);
	}

	void EditorContext::OnUpdate(Engine::Timestep ts)
	{
		for (auto& window : m_Windows)
		{
			window->OnUpdate(ts);
		}
	}

	void EditorContext::OnImGuiRender()
	{
		m_MenuBar.OnImGuiRender();

		for (auto& window : m_Windows)
		{
			window->OnImGuiRender();
		}

		//ImGui::ShowDemoWindow();
	}

	void EditorContext::OnEvent(Engine::Event& e)
	{
		for (auto& window : m_Windows)
		{
			window->OnEvent(e);
		}
	}
}

