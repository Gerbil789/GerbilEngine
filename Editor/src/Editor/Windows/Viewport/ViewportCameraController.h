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

	private:
		void OnMouseScroll(Engine::MouseScrolledEvent& e);
		void OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		void OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e);
		void OnMouseMoved(Engine::MouseMovedEvent& e);
		void OnEntityFocus(Engine::Uuid entityId, float distance = 10.0f);
	};
}