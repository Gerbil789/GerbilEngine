#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/KeyCodes.h"
#include <glm/glm.hpp>

namespace Engine
{
	class Window;
}

namespace Engine::Input
{
	void Initialize(Window& window);
	ENGINE_API bool IsKeyPressed(KeyCode key);
	ENGINE_API bool IsKeyPressed(std::initializer_list<KeyCode> keys);
	ENGINE_API bool IsMouseButtonPressed(MouseCode button);
	ENGINE_API glm::vec2 GetMousePosition();
	ENGINE_API float GetMouseX();
	ENGINE_API float GetMouseY();
}