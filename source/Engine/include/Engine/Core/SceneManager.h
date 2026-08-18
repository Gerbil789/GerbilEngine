#pragma once

#include "Engine/Core/UUID.h"

namespace Engine::SceneManager
{
	ENGINE_API void SetActiveScene(Uuid id);
	ENGINE_API Uuid GetActiveScene();
}