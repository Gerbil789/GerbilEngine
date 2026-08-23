#pragma once

#ifdef __EMSCRIPTEN__
#include <webgpu/webgpu_cpp.h> // The official Emscripten Dawn C++ header
#else
#include <webgpu/webgpu_cpp.h>   // Your local Windows header
#endif

namespace Engine
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