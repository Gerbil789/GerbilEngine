#pragma once

#include <webgpu/webgpu.hpp>

namespace Engine::RenderUtils
{
	struct alignas(16) FrameUniforms {
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 cameraPosition;
		float padding;
	};

	static_assert(sizeof(FrameUniforms) % 16 == 0);


	struct alignas(16) ModelUniforms
	{
		glm::mat4 modelMatrix;
	};

	static_assert(sizeof(ModelUniforms) % 16 == 0);

	void Initialize();

	extern wgpu::Device s_Device;

	extern wgpu::BindGroupLayout s_ModelBindGroupLayout;
	extern wgpu::BindGroupLayout s_FrameBindGroupLayout;


	wgpu::Buffer CreateModelBuffer();
	wgpu::BindGroup CreateModelBindGroup(wgpu::Buffer modelBuffer);

	wgpu::Buffer CreateFrameBuffer();
	wgpu::BindGroup CreateFrameBindGroup(wgpu::Buffer frameBuffer);
}