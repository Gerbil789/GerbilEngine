#pragma once

#include <Engine.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Panels/ContentBrowserPanel.h"


namespace Engine 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::string& filepath);
		void SaveSceneAs();
		void SaveScene();

		void SerializeScene(Ref<Scene> scene, const std::string& filepath);

		void UI_Toolbar();

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicatedEntity();
	private:
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		ShaderLibrary m_ShaderLibrary;
		Ref<VertexArray> m_VertexArrayObject;

		Ref<Texture2D> m_BackroundTexture;
		Ref<Texture2D> m_GerbilTexture;
		Ref<Texture2D> m_TileTexture;
		Ref<Texture2D> m_Spritesheet;
		Ref<Texture2D> m_Icon_Play;
		Ref<Texture2D> m_Icon_Stop;

		std::vector<Ref<SubTexture2D>> m_TileTextures;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;

		std::filesystem::path m_EditorScenePath;

		//panels

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		int m_GizmoType = 7; //translate

		EditorCamera m_EditorCamera;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };

		Entity m_HoveredEntity;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		
		SceneState m_SceneState = SceneState::Edit;

	};
}