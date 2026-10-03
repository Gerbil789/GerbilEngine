#pragma once

#include "Engine/Core/Scene.h"

namespace engine
{
	class TransformSystem
	{
	public:
		static void Update(entt::registry& registry, const std::vector<entt::entity>& rootEntities);
		static const glm::mat4 CalculateLocalPositionMatrix(const TransformComponent& transform);
	};
}