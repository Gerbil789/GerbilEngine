#pragma once

#include "Engine/Core/API.h"
#include <filesystem>
#include <functional>

struct GLFWwindow;

namespace GLFW
{
	ENGINE_API void Initialize();
	ENGINE_API void Shutdown();
}

namespace Engine
{
	class Event;

	enum class WindowMode { Windowed, BorderlessFullscreen };

	struct WindowSpecification
	{
		std::string title = "untitled window";
		uint32_t width = 1600;
		uint32_t height = 900;
		std::filesystem::path iconPath = "";
		WindowMode mode = WindowMode::Windowed;
	};

	class ENGINE_API Window
	{
	public:
		Window(const WindowSpecification& specification);
		~Window();

		uint32_t GetWidth() const { return m_Data.width; }
		uint32_t GetHeight() const { return m_Data.height; }
		void* GetNativeWindow() const { return m_Window; } // GLFWwindow*
		void* GetSurface() const; // wgpu::Surface

		void SetEventCallback(const std::function<void(Event&)>& callback) { m_Data.callback = callback; }

		void SetMode(WindowMode mode);
		WindowMode GetMode() const;

		bool IsMinimized() const { return m_Minimized; }

	private:
		void SetEventCallbacks();
		void ConfigureSurface(uint32_t width, uint32_t height);
		void SetWindowIcon(const std::filesystem::path& iconPath);

	private:
		GLFWwindow* m_Window = nullptr;
		WindowMode m_Mode = WindowMode::Windowed;
		bool m_Minimized = false;

		int m_WindowedX = 100;
		int m_WindowedY = 100;
		int m_WindowedWidth = 1280;
		int m_WindowedHeight = 720;

		struct WindowData
		{
			uint32_t width = 1600, height = 900;
			std::function<void(Event&)> callback;
			Window* window = nullptr;
		} m_Data;
	};
} 