#pragma once

#include "enginepch.h"
#include "Engine/Core/Core.h"
#include "Engine/Events/Event.h"
#include "Engine/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace Engine
{
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const std::string& title = "New Window", uint32_t width = 1600, uint32_t height = 900, std::filesystem::path iconPath = "resources/icons/logo.png");
		~Window();

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		GLFWwindow* GetGLFWWindow() const { return m_Window; }
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title;
			uint32_t Width = 1600, Height = 900;
			std::filesystem::path IconPath;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
} 