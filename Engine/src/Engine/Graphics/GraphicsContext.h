#pragma once

#include <webgpu/webgpu.hpp>

namespace Engine::GraphicsContext
{
	void Initialize();
	void Shutdown();

	const wgpu::Instance& GetInstance();
	const wgpu::Device& GetDevice();
	const wgpu::Queue& GetQueue();

	//TODO: add minimum system requirements for the graphics context
}