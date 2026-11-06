#include "enginepch.h"
#include "Engine/Core/Window.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"
#include <stb_image.h>
#include <GLFW/glfw3.h>

#if !defined(ENGINE_PLATFORM_WEB)
#include <GLFW/glfw3native.h>
#endif

namespace GLFW
{
	void Initialize()
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initialize GLFW");
		}

		glfwSetErrorCallback([](int error, const char* description) { LOG_ERROR("GLFW Error ({}): {}", error, description); });
	}

	void Shutdown()
	{
		glfwTerminate();
	}

	void PollEvents()
	{
		glfwPollEvents();
	}

	void WaitEvents()
	{
		glfwWaitEvents();
	}
}

namespace engine
{
	static wgpu::Surface CreateSurface([[maybe_unused]]GLFWwindow* window)
	{
		wgpu::SurfaceDescriptor surfaceDesc;
		surfaceDesc.label = "WindowSurface";

#if defined(ENGINE_PLATFORM_WINDOWS)
		wgpu::SurfaceSourceWindowsHWND hwndDesc;
		hwndDesc.hwnd = glfwGetWin32Window(window);
		hwndDesc.hinstance = GetModuleHandle(nullptr);
		hwndDesc.sType = wgpu::SType::SurfaceSourceWindowsHWND;
		surfaceDesc.nextInChain = &hwndDesc;
#elif defined(ENGINE_PLATFORM_LINUX)
		wgpu::SurfaceSourceXlibWindow x11Desc;
		x11Desc.sType = wgpu::SType::SurfaceSourceXlibWindow;
		x11Desc.display = glfwGetX11Display();
		x11Desc.window = glfwGetX11Window(window);
		surfaceDesc.nextInChain = &x11Desc;
#elif defined(ENGINE_PLATFORM_WEB)
		wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
		canvasDesc.selector = "#canvas";
		surfaceDesc.nextInChain = &canvasDesc;
#endif
		return GraphicsContext::GetInstance().CreateSurface(&surfaceDesc);
	}

	void Window::Initialize(const WindowSpecification& specification)
	{
		width = specification.width;
		height = specification.height;

		LOG_TRACE("Creating '{}' window ({}, {})", specification.title, width, height);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), specification.title.c_str(), nullptr, nullptr);

		if (!m_window)
		{
			throw std::runtime_error("Could not create GLFW window!");
		}

		glfwSetWindowSizeLimits(m_window, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);

		glfwSetWindowUserPointer(m_window, this);

		SetEventCallbacks();
		SetWindowIcon(specification.iconPath);

		m_surface = CreateSurface(static_cast<GLFWwindow*>(m_window));

		wgpu::SurfaceCapabilities capabilities;
		m_surface.GetCapabilities(GraphicsContext::GetAdapter(), &capabilities);
		GraphicsContext::SetSurfaceFormat(capabilities.formats[0]);

		ConfigureSurface();
	}

	void Window::Shutdown()
	{
		if (m_window)
		{
			glfwDestroyWindow(m_window);
			m_window = nullptr;
		}

		m_surface = nullptr;
	}

	wgpu::Surface Window::GetSurface() const
	{
		return m_surface;
	}

	wgpu::TextureView Window::GetSurfaceView() const
	{
		wgpu::SurfaceTexture surfaceTexture;
		m_surface.GetCurrentTexture(&surfaceTexture);
		if (!(surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal || surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal))
		{
			LOG_WARNING("Surface texture status is not optimal. status: {}", (uint32_t)surfaceTexture.status);
		}

		wgpu::TextureView view = surfaceTexture.texture.CreateView();

		if (!view)
		{
			LOG_ERROR("Failed to create surface texture view");
			return {};
		}
		return view;
	}

	void Window::SetTitle(const std::string& title)
	{
		if (m_window)
		{
			glfwSetWindowTitle(m_window, title.c_str());
		}
	}

	void Window::SetMode(WindowMode mode)
	{
		if (m_mode == mode) return;

		m_mode = mode;

		if (mode == WindowMode::BorderlessFullscreen)
		{
			glfwGetWindowPos(m_window, &m_windowedX, &m_windowedY);
			glfwGetWindowSize(m_window, &m_windowedWidth, &m_windowedHeight);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

			glfwSetWindowAttrib(m_window, GLFW_DECORATED, GLFW_FALSE);

			glfwSetWindowPos(m_window, 0, 0);
			glfwSetWindowSize(m_window, vidmode->width, vidmode->height);
		}
		else
		{
			glfwSetWindowAttrib(m_window, GLFW_DECORATED, GLFW_TRUE);

			glfwSetWindowSize(m_window, m_windowedWidth, m_windowedHeight);
			glfwSetWindowPos(m_window, m_windowedX, m_windowedY);
		}

	}

	WindowMode Window::GetMode() const
	{
		return m_mode;
	}

	void Window::ToggleFullscreen()
	{
		if (m_mode == WindowMode::BorderlessFullscreen)
		{
			SetMode(WindowMode::Windowed);
		}
		else
		{
			SetMode(WindowMode::BorderlessFullscreen);
		}
	}

	void Window::SetEventCallbacks()
	{
		glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
			{
				if (width == 0 || height == 0) { return; }

				Window& self = *(Window*)glfwGetWindowUserPointer(window);
				self.width = static_cast<uint32_t>(width);
				self.height = static_cast<uint32_t>(height);
				self.ConfigureSurface();
				self.sizeChanged = true;
			});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
			{
				Window& self = *(Window*)glfwGetWindowUserPointer(window);
				self.onCloseCallback();
			});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int, int action, int)
			{
				if(action == GLFW_PRESS && key == GLFW_KEY_F11)
				{
					Window& self = *(Window*)glfwGetWindowUserPointer(window);
					self.ToggleFullscreen();
				}
			});

		glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified)
			{
				Window& self = *(Window*)glfwGetWindowUserPointer(window);
				self.minimized = iconified != 0;
			});
	}

	void Window::ConfigureSurface()
	{
		wgpu::SurfaceConfiguration config;
		config.width = width;
		config.height = height;
		config.device = GraphicsContext::GetDevice();
		config.format = GraphicsContext::GetSurfaceFormat();
		config.presentMode = wgpu::PresentMode::Fifo;
		config.alphaMode = wgpu::CompositeAlphaMode::Opaque;

		m_surface.Configure(&config);
	}

	void Window::SetWindowIcon(const std::filesystem::path& path)
	{
		int width, height, channels;
		unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!data)
		{
			LOG_ERROR("Failed to load icon from: {}", path);
			return;
		}

		GLFWimage icon{ width, height, data };
		glfwSetWindowIcon(m_window, 1, &icon);
		stbi_image_free(data);
	}
}