#pragma once
#include <glm/glm.hpp>

namespace Engine 
{
	namespace Math
	{
		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);

		glm::mat4 CreateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);




	}
}