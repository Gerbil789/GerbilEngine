#pragma once

#include <Engine.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Engine/Renderer/EditorCamera.h"


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

		void NewScene();
		void OpenScene();
		void SaveSceneAs();


	private:
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		ShaderLibrary m_ShaderLibrary;
		Ref<VertexArray> m_VertexArrayObject;
		OrthographicCameraController m_CameraController;

		Ref<Texture2D> m_BackroundTexture;
		Ref<Texture2D> m_GerbilTexture;
		Ref<Texture2D> m_TileTexture;
		Ref<Texture2D> m_Spritesheet;

		std::vector<Ref<SubTexture2D>> m_TileTextures;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Scene> m_ActiveScene;

		Entity m_SquareEntity;
		Entity m_CameraEntity;


		//panels

		SceneHierarchyPanel m_SceneHierarchyPanel;

		int m_GizmoType = 7; //translate

		EditorCamera m_EditorCamera;
	};
}