#pragma once

#include <webgpu/webgpu.hpp>

namespace Engine::GraphicsContext
{
	void Initialize();
	void Shutdown();
	void ConfigureSurface(uint32_t width, uint32_t height);

	const wgpu::Instance& GetInstance();
	const wgpu::Device& GetDevice();
	const wgpu::Queue& GetQueue();
	const wgpu::Surface& GetSurface();

	//TODO: add minimum system requirements for the graphics context
}