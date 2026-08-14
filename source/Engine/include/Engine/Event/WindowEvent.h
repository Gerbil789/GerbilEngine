#pragma once

#include "Engine/Event/Event.h"

namespace Engine
{
	struct WindowResizeEvent : public Event
	{
		unsigned int width, height;

		WindowResizeEvent(unsigned int width, unsigned int height) : width(width), height(height) {}
	};

	struct WindowCloseEvent : public Event
	{
		WindowCloseEvent() {}
	};

	struct WindowFocusEvent : public Event
	{
		WindowFocusEvent() {}
	};
}