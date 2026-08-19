#pragma once

#include "Engine/Asset/AssetHandle.h"

namespace Engine::SceneManager
{
	ENGINE_API void SetActiveScene(Scene scene);
	ENGINE_API Scene GetActiveScene();
}