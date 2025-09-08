#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Asset/Asset.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	struct TextureSpecification
	{
		uint32_t width = 1;
		uint32_t height = 1;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm;
		bool generateMipMaps = false;
	};

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const TextureSpecification& specification, const void* data);

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		wgpu::TextureView GetTextureView() const { return m_TextureView; }

	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;
		wgpu::TextureFormat m_TextureFormat;

	};
}