#pragma once

#include <glm/glm.hpp>
#include <limits>

namespace Engine
{
	struct AABB
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
	};
}