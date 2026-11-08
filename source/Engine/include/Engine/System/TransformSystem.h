#pragma once

#include "Engine/Core/Scene.h"

namespace engine
{
	class TransformSystem
	{
	public:
		static void Update(Scene& scene);
		static const glm::mat4 CalculateLocalMatrix(const component::Transform& transform);
	};
}