#include "enginepch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Engine/Core/Application.h"

namespace Engine 
{
	bool Input::IsKeyPressed(KeyCode key)
	{
		auto window = Application::Get().GetWindow().GetGLFWWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = Application::Get().GetWindow().GetGLFWWindow();
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = Application::Get().GetWindow().GetGLFWWindow();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return y;
	}
}