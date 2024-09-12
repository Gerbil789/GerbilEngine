#pragma once

#include <Engine.h>
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Scene/SceneManager.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/MaterialPanel.h"
#include "Panels/SettingsPanel.h"

namespace Engine 
{
	class EditorLayer : public Layer, public ISceneObserver
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;
		void OnSceneChanged() override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicatedEntity();

		void MenuBar();
	private:
		Ref<Scene> m_CurrentScene;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		int m_GizmoType = 7; //translate
		EditorCamera m_EditorCamera;
		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		Entity m_HoveredEntity;

		//panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		InspectorPanel m_InspectorPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		MaterialPanel m_MaterialPanel;
		SettingsPanel m_SettingsPanel;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		Ref<FrameBuffer> m_EditorFrameBuffer;

		glm::vec2 m_GameViewSize = { 0.0f, 0.0f };
		Ref<FrameBuffer> m_GameFrameBuffer;

		// ---- MOVE TO OTHER FILES / TEMP----
		Ref<Texture2D> m_GerbilTexture;
		Ref<Texture2D> m_TileTexture;
		Ref<Texture2D> m_Icon_Play;
		Ref<Texture2D> m_Icon_Pause;
		Ref<Texture2D> m_Icon_Next;
		//Ref<Material> m_Material;
		std::vector<Ref<SubTexture2D>> m_TileTextures;

		float fps = 0.0f;
	};
}