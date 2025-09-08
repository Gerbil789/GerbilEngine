#pragma once

#include "Engine/Event/Event.h"
#include <string>
#include <sstream>

namespace Engine
{
	class WindowEvent : public Event
	{
	public:
		WindowEvent() = default;
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowOpenEvent : public WindowEvent
	{
	public:
		WindowOpenEvent(const std::string& windowName, void* data = nullptr) : m_WindowName(windowName), m_Data(data) {}
		inline const std::string& GetWindowName() const { return m_WindowName; }

		template<typename T>
		T* GetData() const
		{
			return static_cast<T*>(m_Data);
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "OpenWindowEvent: " << m_WindowName << "\n";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowOpen)
	private:
		std::string m_WindowName;
		void* m_Data;
	};
}