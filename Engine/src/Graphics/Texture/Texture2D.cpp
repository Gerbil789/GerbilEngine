#include "enginepch.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine
{
	namespace
	{
		static Texture2D* s_DefaultWhiteTexture = nullptr;
		static Texture2D* s_DefaultNormalTexture = nullptr;
	}

	Texture2D::Texture2D(const TextureSpecification& specification, const void* data)
	{
		m_Width = specification.width;
		m_Height = specification.height;
		m_TextureFormat = specification.format;
		uint32_t mipCount = 1;


		if (specification.generateMips)
		{
			mipCount = GetMaxMipLevelCount({ m_Width , m_Height , 1 });
		}

		uint32_t bytesPerPixel = 4;
		if (m_TextureFormat == wgpu::TextureFormat::RGBA16Float)
		{
			bytesPerPixel = 8;
		}

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.mipLevelCount = mipCount;
		textureDesc.sampleCount = 1;
		textureDesc.size = { m_Width, m_Height, 1 };

		//TODO: write more elegantly
		wgpu::TextureUsage usage = specification.usage;
		if (specification.generateMips)
		{
			usage = specification.usage | wgpu::TextureUsage::StorageBinding;
		}

		textureDesc.usage = usage;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = m_Texture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout;
		layout.offset = 0;
		layout.bytesPerRow = m_Width * bytesPerPixel;
		layout.rowsPerImage = m_Height;

		wgpu::Extent3D size = { m_Width, m_Height, 1 };

		GraphicsContext::GetQueue().writeTexture(dst, data, m_Width * m_Height * bytesPerPixel, layout, size);


		if (specification.generateMips)
		{
			GenerateMipmaps(m_Texture);
		}


		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = { "Texture2DView", WGPU_STRLEN };
		viewDesc.format = m_TextureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
		m_TextureView = m_Texture.createView(viewDesc);
	}

	Texture2D* Texture2D::GetDefault()
	{
		if (!s_DefaultWhiteTexture)
		{
			TextureSpecification spec;
			spec.width = 1;
			spec.height = 1;
			spec.format = wgpu::TextureFormat::RGBA8Unorm;
			uint32_t whitePixel = 0xFFFFFFFF;
			s_DefaultWhiteTexture = new Texture2D(spec, &whitePixel);
		}

		return s_DefaultWhiteTexture;
	}

	Texture2D* Texture2D::GetDefaultNormal()
	{
		if (!s_DefaultNormalTexture)
		{
			TextureSpecification spec;
			spec.width = 1;
			spec.height = 1;
			spec.format = wgpu::TextureFormat::RGBA8Unorm;
			constexpr uint8_t normalPixel[4] = { 128, 128, 255, 255 }; // Blue-ish

			s_DefaultNormalTexture = new Texture2D(spec, &normalPixel);
		}

		return s_DefaultNormalTexture;
	}

	SubTexture2D::SubTexture2D(const Texture2D& texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture), m_UVMin(min), m_UVMax(max) {}

	SubTexture2D SubTexture2D::CreateFromGrid(const Texture2D& texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize)
	{
		glm::vec2 texSize = { static_cast<float>(texture.GetWidth()), static_cast<float>(texture.GetHeight()) };
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

		return SubTexture2D(texture, min, max);
	}

}