#pragma once

#include "IEditorWindow.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Graphics/Renderer/Renderer.h"

namespace Engine
{
	class Event;
}

namespace Editor
{
	class ViewportWindow : public IEditorWindow
	{
	public:
		void Initialize() override;
		void Draw() override;
		void OnEvent(Engine::Event& e);

	private:
		void UpdateViewportSize();

		void DrawGizmos();

	private:
		Engine::Scene* m_Scene = nullptr;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Engine::Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		bool m_GizmoPreviouslyUsed = false;

		std::unordered_map<Engine::Entity, glm::mat4> m_InitialWorldTransforms;
		glm::mat4 m_InitialPrimaryWorld = glm::mat4(1.0f);

	};
}