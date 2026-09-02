#pragma once

#include "Engine/Asset/AssetHandle.h"

namespace engine::SceneManager
{
	void SetActiveScene(Scene scene);
	Scene GetActiveScene();
}