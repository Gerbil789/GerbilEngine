#pragma once

#include <Engine.h>
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Events/WindowEvent.h"
#include "Windows/EditorWindowBase.h"

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
		bool OnWindowOpen(WindowOpenEvent& e);

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

		std::vector<Ref<EditorWindowBase>> m_EditorWindows;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		Ref<FrameBuffer> m_EditorFrameBuffer;

		glm::vec2 m_GameViewSize = { 0.0f, 0.0f };
		Ref<FrameBuffer> m_GameFrameBuffer;

		Ref<Texture2D> m_Icon_Play;
		Ref<Texture2D> m_Icon_Pause;
		Ref<Texture2D> m_Icon_Next;
	};
}