#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu-raii.hpp>

namespace Engine::GraphicsContext
{
	ENGINE_API void Initialize();

	ENGINE_API wgpu::Instance GetInstance();
	ENGINE_API wgpu::Device GetDevice();
	ENGINE_API wgpu::Queue GetQueue();

	ENGINE_API uint32_t GetUniformBufferOffsetAlignment();
	ENGINE_API uint32_t GetStorageBufferOffsetAlignment();

	//TODO: add minimum system requirements for the graphics context
}