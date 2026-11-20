#pragma once

#include <webgpu/webgpu_cpp.h>
#include <array>
#include <glm/glm.hpp>

namespace engine
{
	constexpr static int s_shadowCascadeCount = 4;

	struct alignas(16) ShadowUniforms
	{
		std::array<glm::mat4, s_shadowCascadeCount> lightViewProj = {};
		std::array<float, s_shadowCascadeCount> cascadeSplits = {};
	};
	static_assert(sizeof(ShadowUniforms) % 16 == 0);

	struct alignas(16) ViewUniforms
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 cameraPosition;
		float padding;
	};
	static_assert(sizeof(ViewUniforms) % 16 == 0);
	
	struct Layouts
	{
		static void Initialize();

		inline static wgpu::BindGroupLayout viewBindGroupLayout = nullptr;
		inline static wgpu::BindGroupLayout modelBindGroupLayout = nullptr;
		inline static wgpu::BindGroupLayout environmentBindGroupLayout = nullptr;
	};
}