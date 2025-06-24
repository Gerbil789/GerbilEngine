#pragma once

#include "Editor/Windows/ContentBrowserWindow.h"
#include "Editor/Windows/InspectorWindow.h"
#include "Editor/Windows/SceneHierarchyWindow.h"
#include "Editor/Windows/MaterialWindow.h"
#include "Editor/Windows/SettingsWindow.h"
#include "Editor/Windows/StatisticsWindow.h"
#include "Editor/Windows/MeshImportWindow.h"
#include "Editor/Windows/ViewportWindow.h"
#include "Editor/Windows/GameWindow.h"
#include "Editor/Elements/MenuBar.h"

namespace Editor
{
	class EditorContext
	{
	public:
		EditorContext();

		void OnUpdate(Engine::Timestep ts);
		void OnImGuiRender();
		void OnEvent(Engine::Event& e);

		ViewportWindow* m_ViewportWindow = nullptr;
		GameWindow* m_GameWindow = nullptr;
	private:
		MenuBar m_MenuBar;

		//Windows
		ContentBrowserWindow* m_ContentBrowserWindow = nullptr;
		InspectorWindow* m_InspectorWindow = nullptr;
		SceneHierarchyWindow* m_SceneHierarchyWindow = nullptr;
		MaterialWindow* m_MaterialWindow = nullptr;
		SettingsWindow* m_SettingsWindow = nullptr;
		StatisticsWindow* m_StatisticsWindow = nullptr;
		MeshImportWindow* m_MeshImportWindow = nullptr;


		std::vector<EditorWindow*> m_Windows;
	};


}
