#pragma once

#include <filesystem>
#include <functional>
#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace GLFW
{
	void Initialize();
	void Shutdown();
	void PollEvents();
	void WaitEvents();
}

namespace engine
{
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

		float GetAspectRatio() const { return static_cast<float>(width) / static_cast<float>(height); }
		GLFWwindow* Get() const { return m_window; }
		wgpu::Surface GetSurface() const;
		wgpu::TextureView GetSurfaceView() const;

		void OnClose(const std::function<void()>& callback) { onCloseCallback = callback; }

		void SetTitle(const std::string& title);
		void SetMode(WindowMode mode);
		WindowMode GetMode() const;
		void ToggleFullscreen();

		void ConfigureSurface();

		uint32_t width = 1600;
		uint32_t height = 900;
		bool sizeChanged = true;
		bool minimized = false;

	private:
		void SetEventCallbacks();
		void SetWindowIcon(const std::filesystem::path& path);

	private:
		GLFWwindow* m_window = nullptr;
		wgpu::Surface m_surface;
		WindowMode m_mode = WindowMode::Windowed;

		int m_windowedX = 100;
		int m_windowedY = 100;
		int m_windowedWidth = 1280;
		int m_windowedHeight = 720;

		std::function<void()> onCloseCallback;
	};
} 