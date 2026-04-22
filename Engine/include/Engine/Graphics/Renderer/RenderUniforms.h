#pragma once

#include <array>
#include <glm/glm.hpp>

namespace Engine
{
	constexpr static int s_ShadowCascadeCount = 4;

	struct alignas(16) EnvironmentUniforms
	{
		std::array<glm::mat4, s_ShadowCascadeCount> lightViewProj = {};
		std::array<float, s_ShadowCascadeCount> cascadeSplits = {};
	};
	static_assert(sizeof(EnvironmentUniforms) % 16 == 0);

	struct alignas(16) ViewUniforms 
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 cameraPosition;
		float padding;
	};
	static_assert(sizeof(ViewUniforms) % 16 == 0);

	struct alignas(16) ModelUniforms
	{
		glm::mat4 modelMatrix;
	};
	static_assert(sizeof(ModelUniforms) % 16 == 0);
}