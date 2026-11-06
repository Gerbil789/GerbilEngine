#pragma once

#include <array>
#include <glm/glm.hpp>

namespace engine
{
	constexpr static int s_shadowCascadeCount = 4;

	struct alignas(16) EnvironmentUniforms //TODO: rename to shadow uniforms
	{
		std::array<glm::mat4, s_shadowCascadeCount> lightViewProj = {};
		std::array<float, s_shadowCascadeCount> cascadeSplits = {};
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
}