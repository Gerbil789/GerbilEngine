#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine 
{
	struct KeyPressedEvent : public Event
	{
		Key key;
		int repeatCount;

		KeyPressedEvent(int key, int repeat) : key(static_cast<Key>(key)), repeatCount(repeat) {}
	};

	struct KeyReleasedEvent : public Event
	{
		Key key;

		KeyReleasedEvent(int key) : key(static_cast<Key>(key)) {}
	};

	struct KeyTypedEvent : public Event
	{
		Key key;

		KeyTypedEvent(unsigned int key) : key(static_cast<Key>(key)) {}
	};
}