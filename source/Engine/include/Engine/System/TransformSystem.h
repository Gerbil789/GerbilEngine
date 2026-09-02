#pragma once

#include "Engine/Core/Scene.h"

namespace engine
{
	class TransformSystem
	{
	public:
		static void Update(SceneAsset& scene);
		static const glm::mat4 CalculateLocalPositionMatrix(const TransformComponent& transform);
	};
}