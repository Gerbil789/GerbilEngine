#pragma once

#include "Engine/Event/Event.h"
#include <filesystem>
#include <webgpu/webgpu.hpp>

struct GLFWwindow; // forward declaration

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

		static void InitializeGLFW();
		static void ShutdownGLFW();

		Window(const WindowSpecification& specification);
		~Window();

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.width; }
		uint32_t GetHeight() const { return m_Data.height; }
		void* GetNativeWindow() const { return m_Window; }
		wgpu::Surface GetSurface() const { return m_Surface; }

		void SetEventCallback(const EventCallbackFn& callback) { m_Data.callback = callback; }

	private:
		void SetEventCallbacks();
		void ConfigureSurface(uint32_t width, uint32_t height);
		void SetWindowIcon(const std::filesystem::path& iconPath);

	private:
		GLFWwindow* m_Window = nullptr;
		wgpu::Surface m_Surface;

		struct WindowData
		{
			uint32_t width = 1600, height = 900;
			EventCallbackFn callback;
			Window* window = nullptr;
		} m_Data;
	};
} 