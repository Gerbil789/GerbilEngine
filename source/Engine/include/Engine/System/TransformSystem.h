#pragma once

#include "Engine/Core/Scene.h"

namespace Engine
{
	class TransformSystem
	{
	public:
		static void Update(SceneAsset& scene);
		static const glm::mat4 CalculateLocalPositionMatrix(const TransformComponent& transform);
	};
}