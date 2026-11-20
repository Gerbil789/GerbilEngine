#pragma once

#include <glm/glm.hpp>
#include <limits>

namespace engine
{
	struct AABB
	{
		glm::vec3 min = glm::vec3((std::numeric_limits<float>::max)());
		glm::vec3 max = glm::vec3((std::numeric_limits<float>::lowest)());
	};

	inline AABB TransformAABB(const AABB& local, const glm::mat4& world)
	{
		const glm::vec3 localCenter = (local.min + local.max) * 0.5f;
		const glm::vec3 localExtents = (local.max - local.min) * 0.5f;

		const glm::vec3 worldCenter = glm::vec3(world * glm::vec4(localCenter, 1.0f));

		const glm::vec3 absCol0 = glm::abs(glm::vec3(world[0]));
		const glm::vec3 absCol1 = glm::abs(glm::vec3(world[1]));
		const glm::vec3 absCol2 = glm::abs(glm::vec3(world[2]));

		const glm::vec3 worldExtents
		{
			absCol0.x * localExtents.x + absCol1.x * localExtents.y + absCol2.x * localExtents.z,
			absCol0.y * localExtents.x + absCol1.y * localExtents.y + absCol2.y * localExtents.z,
			absCol0.z * localExtents.x + absCol1.z * localExtents.y + absCol2.z * localExtents.z
		};

		return AABB
		{
			.min = worldCenter - worldExtents,
			.max = worldCenter + worldExtents
		};
	}
}