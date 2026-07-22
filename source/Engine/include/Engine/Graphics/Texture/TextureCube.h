#pragma once

#include <webgpu/webgpu.hpp>

namespace Engine
{
	struct TextureCubeSpecification
	{
		uint32_t size = 1;
		uint32_t mipCount;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA16Float;
	};

	class ENGINE_API TextureCube
	{
	public:
		TextureCube() = default;
		TextureCube(const TextureCubeSpecification& specification);

		TextureCube(TextureCube&&) noexcept = default;
		TextureCube& operator=(TextureCube&&) noexcept = default;

		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		uint32_t GetSize() const { return m_Size; }
		wgpu::Texture GetTexture() const { return m_Texture; }
		wgpu::TextureView GetTextureView() const { return m_TextureView; }
		wgpu::TextureFormat GetFormat() const { return m_TextureFormat; }

	private:
		uint32_t m_Size = 0;
		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;
		wgpu::TextureFormat m_TextureFormat;
	};
}