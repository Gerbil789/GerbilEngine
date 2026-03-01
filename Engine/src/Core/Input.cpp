#include "enginepch.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/Event.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/WindowEvent.h"
#include <GLFW/glfw3.h>

namespace Engine::Input
{
	namespace
	{
		GLFWwindow* s_ActiveWindow = nullptr;

		struct InputState
		{
			std::array<bool, GLFW_KEY_LAST + 1> KeyDown{};
			std::array<bool, GLFW_KEY_LAST + 1> KeyPressed{};
			std::array<bool, GLFW_KEY_LAST + 1> KeyReleased{};
			std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> MouseDown{};
		};

		std::unordered_map<GLFWwindow*, InputState> s_States;
	}

	void SetActiveWindow(GLFWwindow& window)
	{
		s_ActiveWindow = &window;
	}

	void Update()
	{
		for (auto& [window, state] : s_States)
		{
			state.KeyPressed.fill(false);
			state.KeyReleased.fill(false);
		}

		glfwPollEvents();
	}

	void OnEvent(Event& e)
	{
		if (!s_ActiveWindow) return;

		auto& state = s_States[s_ActiveWindow];

		Engine::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<Engine::KeyPressedEvent>([&state](auto& e)
			{
				int key = static_cast<int>(e.GetKey());
				if (!state.KeyDown[key]) state.KeyPressed[key] = true;
				state.KeyDown[key] = true;
			});

		dispatcher.Dispatch<Engine::KeyReleasedEvent>([&state](auto& e)
			{
				int key = static_cast<int>(e.GetKey());
				state.KeyDown[key] = false;
				state.KeyReleased[key] = true;
			});

		dispatcher.Dispatch<Engine::MouseButtonPressedEvent>([&state](auto& e)
			{
				int button = static_cast<int>(e.GetMouseButton());
				state.MouseDown[button] = true;
			});

		dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>([&state](auto& e)
			{
				int button = static_cast<int>(e.GetMouseButton());
				state.MouseDown[button] = false;
			});

		//TODO: handle change focus
		//else if (e.GetEventType() == EventType::WindowFocus)
		//{
		//	auto& ev = static_cast<WindowFocusEvent&>(e);
		//	if (ev.IsFocused())
		//		s_ActiveWindow = ev.GetWindow();
		//}
	}

	bool IsKeyDown(KeyCode key)
	{
		return s_States[s_ActiveWindow].KeyDown[(int)key];
	}

	bool IsKeyPressedOnce(KeyCode key)
	{
		return s_States[s_ActiveWindow].KeyPressed[(int)key];
	}

	bool IsKeyReleased(KeyCode key)
	{
		return s_States[s_ActiveWindow].KeyReleased[(int)key];
	}

	bool IsMouseButtonPressed(MouseCode button)
	{
		return s_States[s_ActiveWindow].MouseDown[(int)button];
	}

	glm::vec2 GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(s_ActiveWindow, &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}
}