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

	ENGINE_API void SetActiveWindow(GLFWwindow& window);
	ENGINE_API void Update();
	//ENGINE_API void OnEvent(Event& e);

	ENGINE_API bool IsKeyDown(Key key);
	ENGINE_API bool IsKeyPressedOnce(Key key);
	ENGINE_API bool IsKeyReleased(Key key);

	ENGINE_API bool IsMouseButtonPressed(Mouse button);
	ENGINE_API glm::vec2 GetMousePosition();

	ENGINE_API void SetCursorMode(CursorMode mode);
	ENGINE_API CursorMode GetCursorMode();
	ENGINE_API void SetCursorPosition(const glm::vec2& position);

	//TODO: mouse scroll, ...
}