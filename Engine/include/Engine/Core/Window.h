#pragma once

#include "Engine/Event/Event.h"
#include <filesystem>
//#include <webgpu/webgpu.hpp>
#include "Engine/Core/API.h"

struct GLFWwindow; // forward declaration

namespace Engine
{
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
		using EventCallbackFn = std::function<void(Event&)>;

		static void InitializeGLFW();
		static void ShutdownGLFW();

		Window(const WindowSpecification& specification);
		~Window();

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.width; }
		uint32_t GetHeight() const { return m_Data.height; }
		void* GetNativeWindow() const { return m_Window; }
		void* GetSurface() const; //wgpu::Surface

		void SetEventCallback(const EventCallbackFn& callback) { m_Data.callback = callback; }

		void SetMode(WindowMode mode);
		WindowMode GetMode() const;

	private:
		void SetEventCallbacks();
		void ConfigureSurface(uint32_t width, uint32_t height);
		void SetWindowIcon(const std::filesystem::path& iconPath);

	private:
		GLFWwindow* m_Window = nullptr;
		WindowMode m_Mode = WindowMode::Windowed;

		// Stored windowed state
		int m_WindowedX = 100;
		int m_WindowedY = 100;
		int m_WindowedWidth = 1280;
		int m_WindowedHeight = 720;

		struct WindowData
		{
			uint32_t width = 1600, height = 900;
			EventCallbackFn callback;
			Window* window = nullptr;
		} m_Data;
	};
} 