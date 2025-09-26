#pragma once
#include <webgpu/webgpu.hpp>

namespace Engine::GraphicsContext
{
	void Initialize();
	void Shutdown();
	void SetWindowSize(uint32_t width, uint32_t height);

	inline wgpu::Instance s_Instance;
	inline wgpu::Device s_Device;
	inline wgpu::Queue s_Queue;
	inline wgpu::Surface s_Surface;

	//TODO: Do i need these getters?
	inline wgpu::Instance GetInstance() { return s_Instance; }
	inline wgpu::Device GetDevice() { return s_Device; }
	inline wgpu::Queue GetQueue() { return s_Queue; }
	inline wgpu::Surface GetSurface() { return s_Surface; }


	//TODO: add system requirements for the graphics context
}