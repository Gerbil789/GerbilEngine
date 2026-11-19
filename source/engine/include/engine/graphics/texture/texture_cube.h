#pragma once

#include <webgpu/webgpu_cpp.h>

namespace engine
{
	struct TextureCubeSpecification
	{
		uint32_t size = 1;
		uint32_t mipCount;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA16Float;
	};

	struct TextureCube
	{
		uint32_t size = 0;
		wgpu::Texture texture;
		wgpu::TextureView textureView;
		wgpu::TextureFormat textureFormat;

		TextureCube() = default;
		TextureCube(const TextureCubeSpecification& specification);

		TextureCube(TextureCube&&) noexcept = default;
		TextureCube& operator=(TextureCube&&) noexcept = default;

		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;
	};
}