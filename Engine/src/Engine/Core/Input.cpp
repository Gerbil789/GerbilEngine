#include "enginepch.h"
#include "Input.h"
#include "Engine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Engine::Input
{
	static GLFWwindow* s_Window = nullptr;

	void Initialize()
	{
		s_Window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
	}

	bool IsKeyPressed(KeyCode key)
	{
		auto state = glfwGetKey(s_Window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool IsKeyPressed(std::initializer_list<KeyCode> keys)
	{
		for (KeyCode key : keys)
		{
			if (IsKeyPressed(key))
				return true;
		}
		return false;
	}

	bool IsMouseButtonPressed(MouseCode button)
	{
		auto state = glfwGetMouseButton(s_Window, button);
		return state == GLFW_PRESS;
	}

	glm::vec2 GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(s_Window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float GetMouseX()
	{
		return GetMousePosition().x;
	}

	float GetMouseY()
	{
		return GetMousePosition().y;
	}
}