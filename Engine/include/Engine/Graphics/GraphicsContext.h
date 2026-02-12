#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu.hpp>

namespace Engine::GraphicsContext
{
	ENGINE_API void Initialize();
	ENGINE_API void Shutdown();

	const wgpu::Instance& GetInstance();
	const wgpu::Device& GetDevice();
	const wgpu::Queue& GetQueue();

	//TODO: add minimum system requirements for the graphics context



	struct GraphicsHandles
	{
		wgpu::Instance instance;
		wgpu::Device device;
		wgpu::Queue queue;
	};

	ENGINE_API GraphicsHandles* GetGraphicsHandles();
}