#pragma once

#include "Engine/Asset/Asset.h"
#include <webgpu/webgpu-raii.hpp>

namespace Engine
{
	struct TextureSpecification
	{
		uint32_t width = 1;
		uint32_t height = 1;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm;
		wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		bool generateMips = false;
	};

	class ENGINE_API Texture : public Asset
	{
	public:
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		wgpu::Texture GetTexture() const { return m_Texture; }
		wgpu::TextureView GetTextureView() const { return m_TextureView; }
		wgpu::TextureFormat GetFormat() const { return m_TextureFormat; }

	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;
		wgpu::TextureFormat m_TextureFormat;
	};
}