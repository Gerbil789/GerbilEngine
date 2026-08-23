#pragma once

#include "Engine/Asset/AssetHandle.h"

namespace Engine::SceneManager
{
	void SetActiveScene(Scene scene);
	Scene GetActiveScene();
}