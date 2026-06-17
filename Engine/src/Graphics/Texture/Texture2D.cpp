#include "enginepch.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
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

	SubTexture2D::SubTexture2D(Uuid texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture), m_UVMin(min), m_UVMax(max) {}

	SubTexture2D SubTexture2D::CreateFromGrid(Uuid texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize)
	{
		const Texture2D& textureAsset = AssetManager::GetAsset<Texture2D>(texture);

		glm::vec2 texSize = { static_cast<float>(textureAsset.GetWidth()), static_cast<float>(textureAsset.GetHeight()) };
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