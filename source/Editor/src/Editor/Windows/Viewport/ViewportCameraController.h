#pragma once

#include <entt/entity/entity.hpp>

namespace engine
{
	struct MouseScrolledEvent;
	struct MouseButtonPressedEvent;
	struct MouseButtonReleasedEvent;
	struct MouseMovedEvent;
}

namespace editor
{
	class ViewportCameraController
	{
	public:
		void Initialize();
		void SetHovered(bool hovered) { m_ViewportHovered = hovered; }

	private:
		void OnMouseScroll(const engine::MouseScrolledEvent& e);
		void OnMouseButtonPressed(const engine::MouseButtonPressedEvent& e);
		void OnMouseButtonReleased(const engine::MouseButtonReleasedEvent& e);
		void OnMouseMoved(const engine::MouseMovedEvent& e);
		void OnEntityFocus(entt::entity entity, float distance = 10.0f); //TODO: this is not getting called anywhere...

		bool m_ViewportHovered = false;
	};
}