#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu-raii.hpp>

namespace Engine::GraphicsContext
{
	ENGINE_API void Initialize();
	ENGINE_API void Shutdown();

	ENGINE_API const wgpu::Instance& GetInstance();
	ENGINE_API const wgpu::Device& GetDevice();
	ENGINE_API const wgpu::Queue& GetQueue();

	ENGINE_API uint32_t GetUniformBufferOffsetAlignment();
	ENGINE_API uint32_t GetStorageBufferOffsetAlignment();

	//TODO: add minimum system requirements for the graphics context
}