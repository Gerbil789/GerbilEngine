#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine 
{
	class KeyPressedEvent : public Event
	{
	public:
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
		EVENT_CLASS_TYPE(KeyPressed)

		KeyPressedEvent(int key, int repeatCount) : m_Key(key), m_RepeatCount(repeatCount) {}
		inline KeyCode GetKey() const { return static_cast<KeyCode>(m_Key); }
		inline int GetRepeatCount() const { return m_RepeatCount; }

	private:
		int m_RepeatCount;
		int m_Key;
	};

	class KeyReleasedEvent : public Event
	{
	public:
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
		EVENT_CLASS_TYPE(KeyReleased)

		KeyReleasedEvent(int key) : m_Key(key) {}
		inline KeyCode GetKey() const { return static_cast<KeyCode>(m_Key); }

	private:
		int m_Key;
	};

	class KeyTypedEvent : public Event
	{
	public:
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
		EVENT_CLASS_TYPE(KeyTyped)

		KeyTypedEvent(int key) : m_Key(key) {}
		inline KeyCode GetKey() const { return static_cast<KeyCode>(m_Key); }

	private:
		int m_Key;
	};
}