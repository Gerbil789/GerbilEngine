#include "enginepch.h"
#include "Texture.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine
{
	static Texture2D* s_DefaultWhiteTexture = nullptr;

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
		wgpu::Texture texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TexelCopyTextureInfo dst{};
		dst.texture = texture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout{};
		layout.offset = 0;
		layout.bytesPerRow = m_Width * 4; // 4 bytes per pixel (RGBA8) TODO: support other formats
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
		m_TextureView = texture.createView(viewDesc);
	}

	Texture2D* Texture2D::GetDefault()
	{
		if(!s_DefaultWhiteTexture)
		{
			TextureSpecification spec{};
			spec.width = 1;
			spec.height = 1;
			spec.format = wgpu::TextureFormat::RGBA8Unorm;
			uint32_t whitePixel = 0xFFFFFFFF;
			s_DefaultWhiteTexture = new Texture2D(spec, &whitePixel);
		}

		return s_DefaultWhiteTexture;
	}

	SubTexture2D::SubTexture2D(Texture2D* texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture), m_UVMin(min), m_UVMax(max) {}

	SubTexture2D* SubTexture2D::CreateFromGrid(Texture2D* texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize)
	{
		glm::vec2 texSize = { (float)texture->GetWidth(), (float)texture->GetHeight() };

		glm::vec2 min
		{
				(cellCoords.x * cellSize.x) / texSize.x,
				(cellCoords.y * cellSize.y) / texSize.y
		};

		glm::vec2 max
		{
				((cellCoords.x + spriteSize.x) * cellSize.x) / texSize.x,
				((cellCoords.y + spriteSize.y) * cellSize.y) / texSize.y
		};

		return new SubTexture2D(texture, min, max);
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
		wgpu::Texture texture = GraphicsContext::GetDevice().createTexture(textureDesc);


    for (uint32_t layer = 0; layer < 6; ++layer)
    {
      wgpu::TexelCopyTextureInfo dst{};
      dst.texture = texture;
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

    m_TextureView = texture.createView(viewDesc);
	}
}