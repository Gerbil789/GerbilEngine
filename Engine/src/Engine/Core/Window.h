#pragma once

#include "enginepch.h"
#include "Engine/Core/Core.h"
#include "Engine/Event/Event.h"
#include <GLFW/glfw3.h>

namespace Engine
{
	struct WindowSpecification
	{
		std::string title = "untitled window";
		uint32_t width = 1600;
		uint32_t height = 900;
		std::filesystem::path iconPath = "";
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowSpecification& specification);
		~Window();

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
		GLFWwindow* GetNativeWindow() const { return m_Window; }

		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

	private:
		void SetEventCallbacks();

	private:
		GLFWwindow* m_Window = nullptr;

		struct WindowData
		{
			uint32_t Width = 1600, Height = 900;
			EventCallbackFn EventCallback;
		} m_Data;
	};
} 