#pragma once

#include "Engine/Asset/AssetHandle.h"
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

	struct TextureHandle : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		Texture2D& Get() const;

		static const TextureHandle White;
		static const TextureHandle Black;
		static const TextureHandle Normal;
		static const TextureHandle HDR;

		static const TextureHandle EDITOR_ICONS;
		static const TextureHandle DEFAULT_FONT_ATLAS;
		static const TextureHandle EDITOR_THUMBNAIL_ATLAS;
	};

	inline constexpr TextureHandle TextureHandle::White{0};
	inline constexpr TextureHandle TextureHandle::Normal{1};
	inline constexpr TextureHandle TextureHandle::HDR{2};

	inline constexpr TextureHandle TextureHandle::EDITOR_ICONS{3};
	inline constexpr TextureHandle TextureHandle::DEFAULT_FONT_ATLAS{4};
	inline constexpr TextureHandle TextureHandle::EDITOR_THUMBNAIL_ATLAS{5};
}