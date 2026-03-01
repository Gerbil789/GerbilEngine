#pragma once

#include "Engine/Event/Event.h"
#include <string>

namespace Engine
{
	class WindowOpenEvent : public Event
	{
	public:
		WindowOpenEvent(const std::string& windowName, void* data = nullptr) : m_WindowName(windowName), m_Data(data) {}
		inline const std::string& GetWindowName() const { return m_WindowName; }

		template<typename T>
		T* GetData() const
		{
			return static_cast<T*>(m_Data);
		}

		EVENT_CLASS_TYPE(WindowOpen)
	private:
		std::string m_WindowName;
		void* m_Data;
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height) : m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() {}
		EVENT_CLASS_TYPE(WindowFocus)
			EVENT_CLASS_CATEGORY(EventCategoryApplication) //TODO: change category
	};
}