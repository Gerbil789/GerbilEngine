#pragma once

#include <glm/glm.hpp>
#include "EditorWindow.h"
#include "Engine/Scene/SceneManager.h"
#include "../Services/SceneController.h"
#include <Engine.h>


namespace Engine
{
	class ViewportWindow : public EditorWindow, public ISceneObserver
	{
	public:
		ViewportWindow(EditorContext* context);
		~ViewportWindow();

		void OnSceneChanged(Ref<Scene> newScene) override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

	private:
		Ref<Scene> m_Scene;

		Ref<FrameBuffer> m_EditorFrameBuffer;
		Ref<EditorCamera> m_EditorCamera;

		int m_GizmoType = 7; //translate

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;


		SceneController* m_SceneController;
	};
}