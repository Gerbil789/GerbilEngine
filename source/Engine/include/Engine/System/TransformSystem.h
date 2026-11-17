#pragma once

#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"

namespace engine
{
	struct TransformSystem
	{
		static const glm::mat4 CalculateLocalMatrix(const component::Transform& transform);
		static void Update(Scene& scene);
	};
}