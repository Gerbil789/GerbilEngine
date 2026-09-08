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

namespace engine
{
	struct Event;

	enum class WindowMode { Windowed, BorderlessFullscreen };

	struct WindowSpecification
	{
		std::string title = "untitled window";
		uint32_t width = 1600;
		uint32_t height = 900;
		std::filesystem::path iconPath = "resources/icons/logo.png";
		WindowMode mode = WindowMode::Windowed;
	};

	class Window
	{
	public:
		void Initialize(const WindowSpecification& specification);
		void Shutdown();

		uint32_t GetWidth() const { return m_Data.width; }
		uint32_t GetHeight() const { return m_Data.height; }
		float GetAspectRatio() const { return static_cast<float>(m_Data.width) / static_cast<float>(m_Data.height); }
		GLFWwindow* Get() const { return m_Window; }
		wgpu::Surface GetSurface() const;
		wgpu::TextureView GetSurfaceView() const;

		void SetEventCallback(const std::function<void(Event&)>& callback) { m_Data.callback = callback; }

		void SetTitle(const std::string& title);
		void SetMode(WindowMode mode);
		WindowMode GetMode() const;
		void ToggleFullscreen();
		bool IsMinimized() const { return m_Minimized; }
		bool SizeChanged() const { return m_SizeChanged; }
		void ClearResizedFlag() { m_SizeChanged = false; }
		void ConfigureSurface();

	private:
		void SetEventCallbacks();
		void SetWindowIcon(const std::filesystem::path& path);

	private:
		GLFWwindow* m_Window = nullptr;
		WindowMode m_Mode = WindowMode::Windowed;
		bool m_Minimized = false;
		bool m_SizeChanged = true;

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