#pragma once

#include "engine/core/scene.h"
#include "engine/core/components.h"

namespace engine
{
	struct Transformsystem
	{
		static const glm::mat4 CalculateLocalMatrix(const component::Transform& transform);
		static void Update(Scene& scene);
	};
}