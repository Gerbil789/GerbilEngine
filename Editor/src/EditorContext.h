#pragma once

#include "Windows/ContentBrowserWindow.h"
#include "Windows/InspectorWindow.h"
#include "Windows/SceneHierarchyWindow.h"
#include "Windows/MaterialWindow.h"
#include "Windows/SettingsWindow.h"
#include "Windows/StatisticsWindow.h"
#include "Windows/MeshImportWindow.h"
#include "Windows/ViewportWindow.h"
#include "Windows/GameWindow.h"
#include "Elements/MenuBar.h"

namespace Engine
{
	class EditorContext
	{
	public:
		EditorContext();

		void OnUpdate(Timestep ts);
		void OnImGuiRender();
		void OnEvent(Event& e);

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
