#include "enginepch.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Core/Core.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Renderer/Texture.h"


namespace Engine
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_ERROR("GLFW Error ({0}): {1}", error, description);
	}


	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}


	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		ENGINE_PROFILE_FUNCTION();
		Init(props);
	}


	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}


	void WindowsWindow::Init(const WindowProps& props)
	{
		ENGINE_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.IconPath = props.IconPath;

		LOG_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);



		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);

		Texture2DFactory textureFactory;
		auto texture = std::dynamic_pointer_cast<Texture2D>(textureFactory.Load(m_Data.IconPath, 4)); // Force 4 channels (RGBA)
		GLFWimage images[1];
		images[0].width = texture->GetWidth();
		images[0].height = texture->GetHeight();
		images[0].pixels = texture->GetPixelData();
		glfwSetWindowIcon(m_Window, 1, images);

		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});


		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});


		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});


		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}


	void WindowsWindow::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwDestroyWindow(m_Window);
	}


	void WindowsWindow::OnUpdate()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwPollEvents();
		m_Context->SwapBuffers();
	}


	void WindowsWindow::SetVSync(bool enabled)
	{
		ENGINE_PROFILE_FUNCTION();
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}


	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}