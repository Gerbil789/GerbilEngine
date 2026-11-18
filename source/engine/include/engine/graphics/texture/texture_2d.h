#pragma once

#include <filesystem>
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
		uint32_t width = 0;
		uint32_t height = 0;
		wgpu::Texture texture;
		wgpu::TextureView textureView;
		wgpu::TextureFormat textureFormat;

		[[nodiscard]] static Texture2D Create(const std::filesystem::path& path);
		[[nodiscard]] static Texture2D Create(const TextureSpecification& specification, const void* data);

		Texture2D() = default;

		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;

		Texture2D(Texture2D&&) noexcept = default;
		Texture2D& operator=(Texture2D&&) noexcept = default;
	};
}