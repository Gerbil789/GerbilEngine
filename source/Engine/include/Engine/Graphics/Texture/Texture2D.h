#pragma once

#include <webgpu/webgpu_cpp.h>

namespace engine
{
	struct TextureSpecification
	{
		uint32_t width = 1;
		uint32_t height = 1;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm;
		wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		bool generateMips = false;
	};

	struct Texture2D
	{
		Texture2D() = default;
		Texture2D(const TextureSpecification& specification, const void* data);

		uint32_t width = 0;
		uint32_t height = 0;
		wgpu::Texture texture;
		wgpu::TextureView textureView;
		wgpu::TextureFormat textureFormat;
	};
}