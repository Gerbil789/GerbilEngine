#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/KeyCodes.h"
#include <glm/glm.hpp>

struct GLFWwindow;
namespace Engine { class Event; }

namespace Engine::Input
{
	ENGINE_API void SetActiveWindow(GLFWwindow& window);
	ENGINE_API void Update();
	ENGINE_API void OnEvent(Event& e);

	ENGINE_API bool IsKeyDown(KeyCode key);
	ENGINE_API bool IsKeyPressedOnce(KeyCode key);
	ENGINE_API bool IsKeyReleased(KeyCode key);

	ENGINE_API bool IsMouseButtonPressed(MouseCode button);
	ENGINE_API glm::vec2 GetMousePosition();
}