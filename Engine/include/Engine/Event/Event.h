#pragma once

namespace Engine
{
	enum class EventType
	{
		None = 0,
		WindowOpen, WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		FileAdded, FileRemoved, FileModified
	};

	enum EventCategory
	{
		None = 0,											// (000000)
		EventCategoryApplication = 1,	// (000001)
		EventCategoryInput = 2,				// (000010)
		EventCategoryKeyboard = 4,		// (000100)
		EventCategoryMouse = 8,				// (001000)
		EventCategoryMouseButton = 16,// (010000) 
		EventCategoryFile = 32				// (100000)
	};


#define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::type; } \
    virtual EventType GetEventType() const override { return GetStaticType(); }

#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
	public:
		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;
	};
}