#include "enginepch.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Event/EventBus.h"
#include <GLFW/glfw3.h>

namespace engine::Input
{
	namespace
	{
		struct InputState
		{
			std::array<bool, GLFW_KEY_LAST + 1> KeyDown{};
			std::array<bool, GLFW_KEY_LAST + 1> KeyPressed{};
			std::array<bool, GLFW_KEY_LAST + 1> KeyReleased{};
			std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> MouseDown{};
		};

		GLFWwindow* s_Window;
		InputState s_State;
	}

	void Initialize(GLFWwindow* window)
	{
		s_Window = window;

		EventBus::Subscribe<KeyPressedEvent>([](auto& e) 
			{
				int key = static_cast<int>(e.key);
				if (!s_State.KeyDown[key]) s_State.KeyPressed[key] = true;
				s_State.KeyDown[key] = true;
				return false;
			});

		EventBus::Subscribe<KeyReleasedEvent>([](auto& e)
			{
				int key = static_cast<int>(e.key);
				s_State.KeyDown[key] = false;
				s_State.KeyReleased[key] = true;
				return false;
			});

		EventBus::Subscribe<MouseButtonPressedEvent>([](auto& e)
			{
				int button = static_cast<int>(e.button);
				s_State.MouseDown[button] = true;
				return false;
			});

		EventBus::Subscribe<MouseButtonReleasedEvent>([](auto& e)
			{
				int button = static_cast<int>(e.button);
				s_State.MouseDown[button] = false;
				return false;
			});

		//TODO: handle change focus
		//else if (e.GetEventType() == EventType::WindowFocus)
		//{
		//	auto& ev = static_cast<WindowFocusEvent&>(e);
		//	if (ev.IsFocused())
		//		s_ActiveWindow = ev.GetWindow();
		//}
	}

	void Update()
	{
		s_State.KeyPressed.fill(false);
		s_State.KeyReleased.fill(false);
		glfwPollEvents();
	}

	bool IsKeyDown(Key key)
	{
		return s_State.KeyDown[(int)key];
	}

	bool IsKeyPressedOnce(Key key)
	{
		return s_State.KeyPressed[(int)key];
	}

	bool IsKeyReleased(Key key)
	{
		return s_State.KeyReleased[(int)key];
	}

	bool IsMouseButtonPressed(Mouse button)
	{
		return s_State.MouseDown[(int)button];
	}

	glm::vec2 GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(s_Window, &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
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
		glfwSetInputMode(s_Window, GLFW_CURSOR, glfwMode);
		
	}

	CursorMode GetCursorMode()
	{
		int glfwMode = glfwGetInputMode(s_Window, GLFW_CURSOR);
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
		glfwSetCursorPos(s_Window, position.x, position.y);
	}
}