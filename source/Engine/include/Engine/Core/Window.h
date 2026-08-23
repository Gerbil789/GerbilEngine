#pragma once

#include <filesystem>
#include <functional>
#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace GLFW
{
	void Initialize();
	void Shutdown();
	void WaitEvents();
}

namespace Engine
{
	struct Event;

	enum class WindowMode { Windowed, BorderlessFullscreen };

	struct WindowSpecification
	{
		std::string title = "untitled window";
		uint32_t width = 1600;
		uint32_t height = 900;
		std::filesystem::path iconPath = "";
		WindowMode mode = WindowMode::Windowed;
	};

	class Window
	{
	public:
		void Initialize(const WindowSpecification& specification);
		void Shutdown();

		uint32_t GetWidth() const { return m_Data.width; }
		uint32_t GetHeight() const { return m_Data.height; }
		GLFWwindow* GetNativeWindow() const { return m_Window; }
		wgpu::Surface GetSurface() const;

		void SetEventCallback(const std::function<void(Event&)>& callback) { m_Data.callback = callback; }

		void SetTitle(const std::string& title);
		void SetMode(WindowMode mode);
		WindowMode GetMode() const;
		void ToggleFullscreen();
		bool IsMinimized() const { return m_Minimized; }

	private:
		void SetEventCallbacks();
		void ConfigureSurface(uint32_t width, uint32_t height);
		void SetWindowIcon(const std::filesystem::path& path);

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
			Window* self = nullptr;
		} m_Data;
	};
} 