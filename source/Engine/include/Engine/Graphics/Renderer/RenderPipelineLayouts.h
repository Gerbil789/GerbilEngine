#pragma once

#include <webgpu/webgpu_cpp.h>

namespace engine
{
	class RenderPipelineLayouts
	{
	public:
		static void Initialize();

		static wgpu::BindGroupLayout GetViewLayout();
		static wgpu::BindGroupLayout GetModelLayout();
		static wgpu::BindGroupLayout GetEnvironmentLayout();
	};
}