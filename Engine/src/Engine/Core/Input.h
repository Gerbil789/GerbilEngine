#pragma once

#include "Engine/Core/KeyCodes.h"
#include <glm/glm.hpp>

namespace Engine::Input
{
	void Init();
	bool IsKeyPressed(KeyCode key);
	bool IsKeyPressed(std::initializer_list<KeyCode> keys);
	bool IsMouseButtonPressed(MouseCode button);
	glm::vec2 GetMousePosition();
	float GetMouseX();
	float GetMouseY();
}