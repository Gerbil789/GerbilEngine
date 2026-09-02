#include "enginepch.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace engine
{
	Texture2DAsset::Texture2DAsset(const TextureSpecification& specification, const void* data)
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
		textureDesc.dimension = wgpu::TextureDimension::e2D;
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
		m_Texture = GraphicsContext::GetDevice().CreateTexture(&textureDesc);

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

		GraphicsContext::GetQueue().WriteTexture(&dst, data, m_Width * m_Height * bytesPerPixel, &layout, &size);

		if (specification.generateMips)
		{
			GenerateMipmaps(m_Texture);
		}

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = "Texture2DView";
		viewDesc.format = m_TextureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::e2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
		m_TextureView = m_Texture.CreateView(&viewDesc);
	}
}