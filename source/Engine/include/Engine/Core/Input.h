#pragma once

#include "Engine/Core/KeyCodes.h"
#include <glm/glm.hpp>

struct GLFWwindow;
namespace Engine 
{ 
	struct Event; 

}

namespace Engine::Input
{
	enum class CursorMode { Normal, Hidden, Disabled };

	void SetActiveWindow(GLFWwindow& window);
	void Update();
	//ENGINE_API void OnEvent(Event& e);

	bool IsKeyDown(Key key);
	bool IsKeyPressedOnce(Key key);
	bool IsKeyReleased(Key key);

	bool IsMouseButtonPressed(Mouse button);
	glm::vec2 GetMousePosition();

	void SetCursorMode(CursorMode mode);
	CursorMode GetCursorMode();
	void SetCursorPosition(const glm::vec2& position);

	//TODO: mouse scroll, ...
}