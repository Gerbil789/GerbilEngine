#pragma once

#include <webgpu/webgpu_cpp.h>

namespace Engine::GraphicsContext
{
	void Initialize();
	void Shutdown();

	wgpu::Adapter GetAdapter();
	wgpu::Instance GetInstance();
	wgpu::Device GetDevice();
	wgpu::Queue GetQueue();

	uint32_t GetUniformBufferOffsetAlignment();
	uint32_t GetStorageBufferOffsetAlignment();

	void SetSurfaceFormat(wgpu::TextureFormat format);
	wgpu::TextureFormat GetSurfaceFormat();

	//TODO: add minimum system requirements for the graphics context
}