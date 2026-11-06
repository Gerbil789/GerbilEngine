#include "enginepch.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Window.h"
#include <GLFW/glfw3.h>

namespace engine::input
{
	namespace
	{
		struct InputState
		{
			std::array<bool, GLFW_KEY_LAST + 1> KeyDown{};
			std::array<bool, GLFW_KEY_LAST + 1> KeyPressed{};
			std::array<bool, GLFW_KEY_LAST + 1> KeyReleased{};

			std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> MouseDown{};
			std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> MousePressed{};
			std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> MouseReleased{};

			float ScrollX = 0.0f;
			float ScrollY = 0.0f;
		};

		GLFWwindow* s_window;
		InputState s_state;
	}

	void Initialize(Window& window)
	{
		s_window = window.Get();

		glfwSetKeyCallback(s_window, [](GLFWwindow*, int key, int, int action, int)
			{
				switch (action)
				{
				case GLFW_PRESS:
				{
					if (!s_state.KeyDown[key]) s_state.KeyPressed[key] = true;
					s_state.KeyDown[key] = true;
					break;
				}
				case GLFW_RELEASE:
				{
					s_state.KeyDown[key] = false;
					s_state.KeyReleased[key] = true;
					break;
				}
				}
			});


		glfwSetMouseButtonCallback(s_window, [](GLFWwindow*, int button, int action, int)
			{
				switch (action)
				{
				case GLFW_PRESS:
				{
					if (!s_state.MouseDown[button]) s_state.MousePressed[button] = true;
					s_state.MouseDown[button] = true;
					break;
				}
				case GLFW_RELEASE:
				{
					s_state.MouseDown[button] = false;
					s_state.MouseReleased[button] = true;
					break;
				}
				}
			});

		glfwSetScrollCallback(s_window, [](GLFWwindow*, double xOffset, double yOffset)
			{
				s_state.ScrollX += static_cast<float>(xOffset);
				s_state.ScrollY += static_cast<float>(yOffset);
			});
	}

	void Update()
	{
		s_state.KeyPressed.fill(false);
		s_state.KeyReleased.fill(false);
		s_state.MousePressed.fill(false);
		s_state.MouseReleased.fill(false);
		s_state.ScrollX = 0.0f;
    s_state.ScrollY = 0.0f;
		GLFW::PollEvents();
	}

	bool IsKeyDown(Key key)
	{
		return s_state.KeyDown[static_cast<int>(key)];
	}

	bool IsKeyPressed(Key key)
	{
		return s_state.KeyPressed[static_cast<int>(key)];
	}

	bool IsKeyReleased(Key key)
	{
		return s_state.KeyReleased[static_cast<int>(key)];
	}

	bool IsMouseButtonDown(Mouse button)
	{
		return s_state.MouseDown[static_cast<int>(button)];
	}

	bool IsMouseButtonPressed(Mouse button)
	{
		return s_state.MouseDown[static_cast<int>(button)];
	}

	bool IsMouseButtonReleased(Mouse button)
	{
		return s_state.MouseReleased[static_cast<int>(button)];
	}

	glm::vec2 GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(s_window, &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	glm::vec2 GetMouseScroll()
  {
    return { s_state.ScrollX, s_state.ScrollY };
  }

	void SetCursorMode(CursorMode mode)
	{
		int glfwMode = GLFW_CURSOR_NORMAL;
		switch (mode)
		{
		case CursorMode::Normal: glfwMode = GLFW_CURSOR_NORMAL; break;
		case CursorMode::Hidden: glfwMode = GLFW_CURSOR_HIDDEN; break;
		case CursorMode::Disabled: glfwMode = GLFW_CURSOR_DISABLED; break;
		}
		glfwSetInputMode(s_window, GLFW_CURSOR, glfwMode);

	}

	CursorMode GetCursorMode()
	{
		int glfwMode = glfwGetInputMode(s_window, GLFW_CURSOR);
		switch (glfwMode)
		{
		case GLFW_CURSOR_NORMAL: return CursorMode::Normal;
		case GLFW_CURSOR_HIDDEN: return CursorMode::Hidden;
		case GLFW_CURSOR_DISABLED: return CursorMode::Disabled;
		default: return CursorMode::Normal; // should not happen
		}
	}

	void SetCursorPosition(const glm::vec2& position)
	{
		glfwSetCursorPos(s_window, position.x, position.y);
	}
}