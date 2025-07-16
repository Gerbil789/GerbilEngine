#pragma once

#include "Engine/Events/Event.h"

namespace Engine 
{
	class KeyEvent : public Event
	{
	public:
		inline int GetKey() const { return m_Key; }

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

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_Key << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};


	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int key) : KeyEvent(key) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_Key;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};


	class KeyTypedEvent : public KeyEvent
	{
		public:
		KeyTypedEvent(int key) : KeyEvent(key) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_Key;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}