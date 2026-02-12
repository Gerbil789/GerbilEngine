#include "enginepch.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Window.h"
#include <GLFW/glfw3.h>

namespace Engine::Input
{
	namespace
	{
		GLFWwindow* s_Window = nullptr;
	}

	void Initialize(Window& window)
	{
		s_Window = static_cast<GLFWwindow*>(window.GetNativeWindow());
	}

	bool IsKeyPressed(KeyCode key)
	{
		int state = glfwGetKey(s_Window, static_cast<int>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool IsMouseButtonPressed(MouseCode button)
	{
		int state = glfwGetMouseButton(s_Window, button);
		return state == GLFW_PRESS;
	}

	glm::vec2 GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(s_Window, &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
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