#pragma once

#include "IEditorWindow.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Editor/Core/EditorCameraController.h"

namespace Editor
{
	class ViewportWindow : public IEditorWindow
	{
	public:
		ViewportWindow();
		void Draw() override;
		void OnEvent(Engine::Event& e) override;

	private:
		void OnKeyPressed(Engine::KeyPressedEvent& e);
		void OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		void UpdateViewportSize();

		void DrawGizmos();

	private:
		Engine::Scene* m_Scene = nullptr;
		Engine::Renderer m_Renderer;

		EditorCameraController m_CameraController;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Engine::Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		bool m_GizmoPreviouslyUsed = false;
		//glm::vec3 m_InitialPos = glm::vec3(0.0f), m_InitialRot = glm::vec3(0.0f), m_InitialScale = glm::vec3(1.0f);

		std::unordered_map<Engine::Entity, glm::mat4> m_InitialWorldTransforms;
		glm::mat4 m_InitialPrimaryWorld = glm::mat4(1.0f);

	};
}