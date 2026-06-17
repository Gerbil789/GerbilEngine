#pragma once

#include "Engine/Graphics/Texture/TextureSpecification.h"

namespace Engine
{
	class ENGINE_API TextureCube
	{
	public:
		TextureCube() = default;
		TextureCube(const TextureSpecification& specification);
		//TextureCube(const TextureSpecification& specification, const void* data);

		TextureCube(TextureCube&&) noexcept = default;
		TextureCube& operator=(TextureCube&&) noexcept = default;

		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		wgpu::Texture GetTexture() const { return m_Texture; }
		wgpu::TextureView GetTextureView() const { return m_TextureView; }
		wgpu::TextureView GetPreviewView() const { return m_PreviewTextureView; }
		wgpu::TextureFormat GetFormat() const { return m_TextureFormat; }

	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;

		wgpu::TextureView m_PreviewTextureView;
		wgpu::TextureFormat m_TextureFormat;
	};
}