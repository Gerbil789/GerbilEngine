#include "enginepch.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/AssetManager.h"

namespace engine
{
	Texture2DData& Texture2D::Get() const
	{
		return AssetManager::GetAsset(*this);
	}

	uint32_t Texture2D::Width() const
	{
		return Get().width;
	}

	uint32_t Texture2D::Height() const
	{
		return Get().height;
	}

	wgpu::TextureView Texture2D::View() const
	{
		return Get().textureView;
	}

	wgpu::TextureFormat Texture2D::Format() const
	{
		return Get().textureFormat;
	}

	Texture2DData::Texture2DData(const TextureSpecification &specification, const void *data)
	{
		width = specification.width;
		height = specification.height;
		textureFormat = specification.format;

		uint32_t mipCount = specification.generateMips ? GetMaxMipLevelCount(width, height) : 1;
		uint32_t bytesPerPixel = (textureFormat == wgpu::TextureFormat::RGBA16Float) ? 8 : 4;
		wgpu::Extent3D size = {width, height, 1};

		wgpu::TextureDescriptor desc;
		desc.dimension = wgpu::TextureDimension::e2D;
		desc.format = textureFormat;
		desc.mipLevelCount = mipCount;
		desc.sampleCount = 1;
		desc.size = size;
		desc.usage = specification.usage | (specification.generateMips ? wgpu::TextureUsage::StorageBinding : wgpu::TextureUsage::None);
		desc.viewFormatCount = 0;
		desc.viewFormats = nullptr;
		texture = GraphicsContext::GetDevice().CreateTexture(&desc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = "Texture2DView";
		viewDesc.format = textureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::e2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
		textureView = texture.CreateView(&viewDesc);

		if(!data) return;

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = texture;
		dst.mipLevel = 0;
		dst.origin = {0, 0, 0};
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout;
		layout.offset = 0;
		layout.bytesPerRow = width * bytesPerPixel;
		layout.rowsPerImage = height;

		GraphicsContext::GetQueue().WriteTexture(&dst, data, width * height * bytesPerPixel, &layout, &size);

		if (specification.generateMips)
		{
			GenerateMipmaps(texture);
		}
	}
}