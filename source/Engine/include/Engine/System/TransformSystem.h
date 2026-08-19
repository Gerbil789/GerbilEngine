#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/Scene.h"

namespace Engine
{
	class ENGINE_API TransformSystem
	{
	public:
		static void Update(SceneAsset& scene);
	};
}