#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine
{
	struct MouseMovedEvent : public Event
	{
		double x, y;

		MouseMovedEvent(double x, double y) : x(x), y(y) {}
	};

	struct MouseScrolledEvent : public Event
	{
		double xOffset, yOffset;

		MouseScrolledEvent(double xOffset, double yOffset) : xOffset(xOffset), yOffset(yOffset) {}
	};

	struct MouseButtonEvent : public Event
	{
		Mouse button;

		MouseButtonEvent(int button) : button(static_cast<Mouse>(button)) {}
	};

	struct MouseButtonPressedEvent : public MouseButtonEvent
	{
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}
	};

	struct MouseButtonReleasedEvent : public MouseButtonEvent
	{
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}
	};
}