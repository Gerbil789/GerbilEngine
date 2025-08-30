#include "enginepch.h"
#include "Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Application.h"
#include <stb_image.h>

namespace Engine
{
	Window::Window(const std::string& title, uint32_t width, uint32_t height, const std::filesystem::path& iconPath)
	{
		ENGINE_PROFILE_FUNCTION();

		m_Title = title;
		m_Data.Width = width;
		m_Data.Height = height;
		m_IconPath = iconPath;

		LOG_INFO("Creating window {0} ({1}, {2})", title, width, height);

		bool glfwInitialized = glfwInit();
		ASSERT(glfwInitialized, "Could not initialize GLFW!");

		glfwSetErrorCallback([](int error, const char* description)
			{
				LOG_ERROR("GLFW Error ({0}): {1}", error, description);
			});

		// Make sure GLFW does not initialize any graphics context.
		// This needs to be done explicitly later.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), nullptr, nullptr);
		ASSERT(m_Window, "Could not create GLFW window!");

		int iconWidth, iconHeight, channels;
		unsigned char* iconPixels = stbi_load(iconPath.string().c_str(), &iconWidth, &iconHeight, &channels, 4);

		if (!iconPixels) 
		{
			LOG_ERROR("Failed to load icon from path: {0}", iconPath.string());
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