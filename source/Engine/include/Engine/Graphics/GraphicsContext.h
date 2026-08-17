#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu_cpp.h>

namespace Engine::GraphicsContext
{
	ENGINE_API void Initialize();
	ENGINE_API void Shutdown();

	ENGINE_API wgpu::Adapter GetAdapter();
	ENGINE_API wgpu::Instance GetInstance();
	ENGINE_API wgpu::Device GetDevice();
	ENGINE_API wgpu::Queue GetQueue();

	ENGINE_API uint32_t GetUniformBufferOffsetAlignment();
	ENGINE_API uint32_t GetStorageBufferOffsetAlignment();

	ENGINE_API void SetSurfaceFormat(wgpu::TextureFormat format);
	ENGINE_API wgpu::TextureFormat GetSurfaceFormat();

	//TODO: add minimum system requirements for the graphics context
}