#pragma once
#include <webgpu/webgpu.hpp>

namespace Engine::GraphicsContext
{
	extern wgpu::Instance s_Instance;
	extern wgpu::Device s_Device;
	extern wgpu::Queue s_Queue;
	extern wgpu::Surface s_Surface;

	void Initialize();
	void Shutdown();
	void SetViewport(uint32_t width, uint32_t height);

	inline wgpu::Instance GetInstance() { return s_Instance; }
	inline wgpu::Device GetDevice() { return s_Device; }
	inline wgpu::Queue GetQueue() { return s_Queue; }
	inline wgpu::Surface GetSurface() { return s_Surface; }

	//TODO: add system requirements for the graphics context"
}