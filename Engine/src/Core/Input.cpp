#include "enginepch.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Window.h"
#include <GLFW/glfw3.h>

namespace Engine::Input
{
	GLFWwindow* m_Window = nullptr;

	void Initialize(Window& window)
	{
		m_Window = static_cast<GLFWwindow*>(window.GetNativeWindow());
	}

	bool IsKeyPressed(KeyCode key)
	{
		auto state = glfwGetKey(m_Window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool IsKeyPressed(std::initializer_list<KeyCode> keys)
	{
		for (KeyCode key : keys)
		{
			if (IsKeyPressed(key))
			{
				return true;
			}
		}
		return false;
	}

	bool IsMouseButtonPressed(MouseCode button)
	{
		auto state = glfwGetMouseButton(m_Window, button);
		return state == GLFW_PRESS;
	}

	glm::vec2 GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);
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