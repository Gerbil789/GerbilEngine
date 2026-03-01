#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine 
{
	class KeyEvent : public Event
	{
	public:
		inline KeyCode GetKey() const { return static_cast<KeyCode>(m_Key); }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int key) : m_Key(key) {}
		int m_Key;
	};


	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int key, int repeatCount) : KeyEvent(key), m_RepeatCount(repeatCount) {}
		inline int GetRepeatCount() const { return m_RepeatCount; }

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};


	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int key) : KeyEvent(key) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};


	class KeyTypedEvent : public KeyEvent
	{
		public:
		KeyTypedEvent(int key) : KeyEvent(key) {}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}