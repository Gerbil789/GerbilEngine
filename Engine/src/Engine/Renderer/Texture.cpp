#include "enginepch.h"
#include "Texture.h"
#include "Engine/Renderer/GraphicsContext.h"

namespace Engine
{
	Texture2D::Texture2D(const TextureSpecification& specification, const void* data)
	{
		m_Width = specification.width;
		m_Height = specification.height;
		m_TextureFormat = specification.format;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { m_Width, m_Height, 1 };
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TexelCopyTextureInfo dst{};
		dst.texture = m_Texture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout{};
		layout.offset = 0;
		layout.bytesPerRow = m_Width * 4; // 4 bytes per pixel (RGBA8)
		layout.rowsPerImage = m_Height;

		wgpu::Extent3D size = { m_Width, m_Height, 1 };

		GraphicsContext::GetQueue().writeTexture(dst, data, m_Width * m_Height * 4, layout, size);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.format = m_TextureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
		m_TextureView = m_Texture.createView(viewDesc);
	}

	CubeMapTexture::CubeMapTexture(const TextureSpecification& specification, const std::array<const void*, 6>& data)
	{
    m_Width = specification.width;
    m_Height = specification.height;
    m_TextureFormat = specification.format;

    wgpu::TextureDescriptor textureDesc{};
    textureDesc.dimension = wgpu::TextureDimension::_2D;
    textureDesc.format = m_TextureFormat;
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.size = { m_Width, m_Height, 6 };
    textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
    textureDesc.viewFormatCount = 0;
    textureDesc.viewFormats = nullptr;
    m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);


    for (uint32_t layer = 0; layer < 6; ++layer)
    {
      wgpu::TexelCopyTextureInfo dst{};
      dst.texture = m_Texture;
      dst.mipLevel = 0;
      dst.origin = { 0, 0, layer };
      dst.aspect = wgpu::TextureAspect::All;

      wgpu::TexelCopyBufferLayout layout{};
      layout.offset = 0; 
      layout.bytesPerRow = m_Width * 4;
      layout.rowsPerImage = m_Height;

      wgpu::Extent3D size = { m_Width, m_Height, 1 };
      GraphicsContext::GetQueue().writeTexture(dst, data[layer], m_Width * m_Height * 4, layout, size);
    }

    wgpu::TextureViewDescriptor viewDesc{};
    viewDesc.format = m_TextureFormat;
    viewDesc.dimension = wgpu::TextureViewDimension::Cube;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = 6;
    viewDesc.aspect = wgpu::TextureAspect::All;

    m_TextureView = m_Texture.createView(viewDesc);
	}
}