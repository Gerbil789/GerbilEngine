#pragma once

#include "Engine/Core/API.h"
#ifdef __EMSCRIPTEN__
#include <webgpu/webgpu_cpp.h> // The official Emscripten Dawn C++ header
#else
#include <webgpu/webgpu.hpp>   // Your local Windows header
#endif

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