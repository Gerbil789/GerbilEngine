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

	class TextureCube
	{
	public:
		TextureCube() = default;
		TextureCube(const TextureCubeSpecification& specification);

		TextureCube(TextureCube&&) noexcept = default;
		TextureCube& operator=(TextureCube&&) noexcept = default;

		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		uint32_t GetSize() const { return m_size; }
		wgpu::Texture GetTexture() const { return m_texture; }
		wgpu::TextureView GetTextureView() const { return m_textureView; }
		wgpu::TextureFormat GetFormat() const { return m_textureFormat; }

	private:
		uint32_t m_size = 0;
		wgpu::Texture m_texture;
		wgpu::TextureView m_textureView;
		wgpu::TextureFormat m_textureFormat;
	};
}