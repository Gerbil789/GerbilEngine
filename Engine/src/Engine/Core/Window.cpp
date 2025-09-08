#include "enginepch.h"
#include "Window.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Application.h"
#include <stb_image.h>

namespace Engine
{
	Window::Window(const WindowSpecification& specification)
	{
		ENGINE_PROFILE_FUNCTION();

		m_Title = specification.title;
		m_Data.Width = specification.width;
		m_Data.Height = specification.height;
		m_IconPath = specification.iconPath;

		LOG_INFO("Creating window {0} ({1}, {2})", m_Title, m_Data.Width, m_Data.Height);

		bool glfwInitialized = glfwInit();
		ASSERT(glfwInitialized, "Could not initialize GLFW!");

		glfwSetErrorCallback([](int error, const char* description)
			{
				LOG_ERROR("GLFW Error ({0}): {1}", error, description);
			});

		// Make sure GLFW does not initialize any graphics context.
		// This needs to be done explicitly later.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Title.c_str(), nullptr, nullptr);

		ASSERT(m_Window, "Could not create GLFW window!");

		int iconWidth, iconHeight, channels;
		unsigned char* iconPixels = stbi_load(m_IconPath.string().c_str(), &iconWidth, &iconHeight, &channels, 4);

		if (!iconPixels) 
		{
			LOG_ERROR("Failed to load icon from path: {0}", m_IconPath.string());
		}
		else 
		{
			GLFWimage icon;
			icon.width = iconWidth;
			icon.height = iconHeight;
			icon.pixels = iconPixels;

			glfwSetWindowIcon(m_Window, 1, &icon);

			stbi_image_free(iconPixels); // Free the image data after setting
		}

		SetEventCallbacks();
	}

	Window::~Window()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::OnUpdate()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwPollEvents();
	}

	void Window::SetEventCallbacks()
	{
		glfwSetWindowUserPointer(m_Window, &m_Data);

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

}