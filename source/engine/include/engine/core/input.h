#pragma once

#include "engine/core/key_codes.h"
#include <glm/glm.hpp>

namespace engine { struct Window; }

namespace engine::input
{
	enum class CursorMode { Normal, Hidden, Disabled };

	void Initialize(Window& window);
	void Update();

	bool IsKeyDown(Key key);
	bool IsKeyPressed(Key key);
	bool IsKeyReleased(Key key);

	bool IsMouseButtonDown(Mouse button);
	bool IsMouseButtonPressed(Mouse button);
	bool IsMouseButtonReleased(Mouse button);
	
	glm::vec2 GetMousePosition();
	glm::vec2 GetMouseScroll();

	void SetCursorMode(CursorMode mode);
	CursorMode GetCursorMode();
	void SetCursorPosition(const glm::vec2& position);
}