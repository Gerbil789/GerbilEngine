#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu-raii.hpp>

namespace Engine
{
	class ENGINE_API RenderPipelineLayouts
	{
	public:
		static void Initialize();

		static wgpu::BindGroupLayout GetViewLayout();
		static wgpu::BindGroupLayout GetModelLayout();
		static wgpu::BindGroupLayout GetEnvironmentLayout();
	};
}