#pragma once

#include <webgpu/webgpu_cpp.h>

namespace engine
{
	struct textureCubeSpecification
	{
		uint32_t size = 1;
		uint32_t mipCount;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA16Float;
	};

	class textureCube
	{
	public:
		textureCube() = default;
		textureCube(const textureCubeSpecification& specification);

		textureCube(textureCube&&) noexcept = default;
		textureCube& operator=(textureCube&&) noexcept = default;

		textureCube(const textureCube&) = delete;
		textureCube& operator=(const textureCube&) = delete;

		uint32_t GetSize() const { return m_size; }
		wgpu::Texture Gettexture() const { return m_texture; }
		wgpu::TextureView GettextureView() const { return m_textureView; }
		wgpu::TextureFormat GetFormat() const { return m_textureFormat; }

	private:
		uint32_t m_size = 0;
		wgpu::Texture m_texture;
		wgpu::TextureView m_textureView;
		wgpu::TextureFormat m_textureFormat;
	};
}