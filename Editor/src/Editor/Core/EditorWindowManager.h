#pragma once

#include "Editor/Elements/MenuBar.h"
#include "Editor/Windows/ContentBrowserWindow.h"
#include "Editor/Windows/InspectorWindow.h"
#include "Editor/Windows/SceneHierarchyWindow.h"
#include "Editor/Windows/MaterialWindow.h"
#include "Editor/Windows/SettingsWindow.h"
#include "Editor/Windows/StatisticsWindow.h"
#include "Editor/Windows/MeshImportWindow.h"
#include "Editor/Windows/ViewportWindow.h"
#include "Editor/Windows/GameWindow.h"

namespace Editor
{
	class EditorWindowManager
	{
	public:
		EditorWindowManager();
		~EditorWindowManager();

		void OnUpdate(Engine::Timestep ts);
		void OnEvent(Engine::Event& e);

		void BlockEvents(bool block) { m_BlockEvents = block; }
		static void ResetLayout(); // TODO: is this good? i dont like static here

	private:
		void BeginFrame();
		void EndFrame();

	private:

		bool m_BlockEvents = true;

		MenuBar m_MenuBar;

		//Windows
		ViewportWindow* m_ViewportWindow = nullptr;
		GameWindow* m_GameWindow = nullptr;
		ContentBrowserWindow* m_ContentBrowserWindow = nullptr;
		InspectorWindow* m_InspectorWindow = nullptr;
		SceneHierarchyWindow* m_SceneHierarchyWindow = nullptr;
		MaterialWindow* m_MaterialWindow = nullptr;
		SettingsWindow* m_SettingsWindow = nullptr;
		StatisticsWindow* m_StatisticsWindow = nullptr;
		MeshImportWindow* m_MeshImportWindow = nullptr;

		std::vector<EditorWindow*> m_Windows; // for easy iteration

		template<typename T, typename... Args>
		T* RegisterWindow(Args&&... args)
		{
			T* window = new T(this, std::forward<Args>(args)...);
			m_Windows.push_back(window);
			return window;
		}
	};
}
