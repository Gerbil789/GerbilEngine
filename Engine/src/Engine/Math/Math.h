#pragma once
#include <glm/glm.hpp>

//TODO: is this file needed?
namespace Engine::Math
{
	constexpr float PI = 3.14159265358979323846f;
	
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
	glm::mat4 CreateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
}