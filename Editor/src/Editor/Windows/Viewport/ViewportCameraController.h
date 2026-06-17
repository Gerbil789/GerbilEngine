#pragma once

#include "Engine/Core/UUID.h"

namespace Engine
{
	struct MouseScrolledEvent;
	struct MouseButtonPressedEvent;
	struct MouseButtonReleasedEvent;
	struct MouseMovedEvent;
}

namespace Editor
{
	class ViewportCameraController
	{
	public:
		void Initialize();
		void SetHovered(bool hovered) { m_ViewportHovered = hovered; }

	private:
		void OnMouseScroll(Engine::MouseScrolledEvent& e);
		void OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		void OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e);
		void OnMouseMoved(Engine::MouseMovedEvent& e);
		void OnEntityFocus(Engine::Uuid entityId, float distance = 10.0f); //TODO: this is not getting called anywhere...

		bool m_ViewportHovered = false;
	};
}