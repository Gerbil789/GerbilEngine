#include "enginepch.h"
#include "Window.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Graphics/GraphicsContext.h"
#include <stb_image.h>
#include <GLFW/glfw3.h>

//TODO: move platform specific code to separate files or figure something better xd
// look at this https://dawn.googlesource.com/dawn/+/refs/heads/main/src/dawn/glfw/utils.cpp

#if defined(ENGINE_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(ENGINE_PLATFORM_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#include <GLFW/glfw3native.h>

namespace Engine
{
	wgpu::Surface CreateSurface(GLFWwindow* window)
	{
		wgpu::SurfaceDescriptor surfaceDesc{};
		surfaceDesc.label = { "MainSurface", WGPU_STRLEN };

#if defined(ENGINE_PLATFORM_WINDOWS)
		wgpu::SurfaceSourceWindowsHWND hwndDesc{};
		hwndDesc.hwnd = glfwGetWin32Window(window);
		hwndDesc.hinstance = GetModuleHandle(nullptr);
		hwndDesc.chain.sType = wgpu::SType::SurfaceSourceWindowsHWND;
		surfaceDesc.nextInChain = &hwndDesc.chain;
#else
		wgpu::SurfaceSourceXlibWindow x11Desc{};
		x11Desc.chain.sType = wgpu::SType::SurfaceSourceXlibWindow;
		x11Desc.display = glfwGetX11Display();
		x11Desc.window = glfwGetX11Window(window);
		surfaceDesc.nextInChain = &x11Desc.chain;
#endif
		return GraphicsContext::GetInstance().createSurface(surfaceDesc);
	}

	void Window::InitializeGLFW()
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Could not initialize GLFW!");
		}

		glfwSetErrorCallback([](int error, const char* description) { LOG_ERROR("GLFW Error ({}): {}", error, description); });
	}

	void Window::ShutdownGLFW()
	{
		glfwTerminate();
	}

	Window::Window(const WindowSpecification& specification)
	{
		m_Data.width = specification.width;
		m_Data.height = specification.height;

		LOG_TRACE("Creating '{}' window ({}, {})", specification.title, m_Data.width, m_Data.height);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(static_cast<int>(m_Data.width), static_cast<int>(m_Data.height), specification.title.c_str(), nullptr, nullptr);

		if(!m_Window)
		{
			throw std::runtime_error("Could not create GLFW window!");
		}

		m_Data.window = this;
		glfwSetWindowUserPointer(m_Window, &m_Data);

		SetEventCallbacks();
		SetWindowIcon(specification.iconPath);

		m_Surface = CreateSurface(static_cast<GLFWwindow*>(m_Window));
		ConfigureSurface(m_Data.width, m_Data.height);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
	}

	void Window::SetMode(WindowMode mode)
	{
		if (m_Mode == mode) 			return;

		m_Mode = mode;

		if (mode == WindowMode::BorderlessFullscreen)
		{
			glfwGetWindowPos(m_Window, &m_WindowedX, &m_WindowedY);
			glfwGetWindowSize(m_Window, &m_WindowedWidth, &m_WindowedHeight);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

			glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_FALSE);

			glfwSetWindowPos(m_Window, 0, 0);
			glfwSetWindowSize(m_Window, vidmode->width, vidmode->height);
		}
		else
		{
			glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);

			glfwSetWindowSize(m_Window, m_WindowedWidth, m_WindowedHeight);
			glfwSetWindowPos(m_Window, m_WindowedX, m_WindowedY);
		}

	}

	WindowMode Window::GetMode() const
	{
		return m_Mode;
	}

	void Window::SetEventCallbacks()
	{
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				Window* self = static_cast<Window*>(data.window);
				self->ConfigureSurface(width, height);

				WindowResizeEvent event(width, height);
				data.callback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.callback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int, int action, int)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.callback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.callback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.callback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.callback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.callback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.callback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.callback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.callback(event);
			});
	}

	void Window::ConfigureSurface(uint32_t width, uint32_t height)
	{
		wgpu::SurfaceConfiguration config{};
		config.width = width;
		config.height = height;
		config.device = GraphicsContext::GetDevice();
		config.format = wgpu::TextureFormat::RGBA8Unorm;
		config.usage = wgpu::TextureUsage::RenderAttachment;
		config.presentMode = wgpu::PresentMode::Fifo;
		config.alphaMode = wgpu::CompositeAlphaMode::Opaque;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;
		config.nextInChain = nullptr;

		m_Surface.configure(config);
	}

	void Window::SetWindowIcon(const std::filesystem::path& iconPath)
	{
		int iconWidth, iconHeight, channels;
		unsigned char* iconPixels = stbi_load(iconPath.string().c_str(), &iconWidth, &iconHeight, &channels, STBI_rgb_alpha);

		if (!iconPixels)
		{
			LOG_ERROR("Failed to load icon from path: {}", iconPath);
			return;
		}

		GLFWimage icon;
		icon.width = iconWidth;
		icon.height = iconHeight;
		icon.pixels = iconPixels;
		glfwSetWindowIcon(m_Window, 1, &icon);
		stbi_image_free(iconPixels);
	}
}