#include "enginepch.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/AssetManager.h"

#include "Engine/Graphics/GraphicsContext.h"
#include <stb_image.h>
#include <glm/gtc/packing.hpp>

namespace engine
{
	Texture2D Texture2D::Create(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Texture path does not exist: {}", path);
			return {};
		}

		void* stbiData = nullptr;
		std::vector<uint16_t> halfFloatData;

		int width = 0;
		int height = 0;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm;

		int channels = 0;

		if (stbi_is_hdr(path.string().c_str()))
		{
			float* data32 = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			stbiData = data32;

			if (data32)
			{
				size_t elementCount = width * height * 4;
				halfFloatData.resize(elementCount);

				const float MAX_HALF_FLOAT = 65504.0f;

				for (size_t i = 0; i < elementCount; ++i)
				{
					float safeValue = std::clamp(data32[i], 0.0f, MAX_HALF_FLOAT);
					halfFloatData[i] = glm::packHalf1x16(safeValue);
				}
				format = wgpu::TextureFormat::RGBA16Float;
			}
		}
		else
		{
			stbiData = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			format = wgpu::TextureFormat::RGBA8Unorm;
		}

		if (!stbiData)
		{
			LOG_ERROR("Failed to load texture at: {}", path);
			LOG_ERROR("stb_image error: {}", stbi_failure_reason());
			return {};
		}

		const void* data = halfFloatData.empty() ? stbiData : halfFloatData.data();

		const TextureSpecification spec {.width = static_cast<uint32_t>(width), .height = static_cast<uint32_t>(height), .format = format};
		Texture2D texture = Create(spec, data);

		if (stbiData) stbi_image_free(stbiData);

		return texture;
	}

	Texture2D Texture2D::Create(const TextureSpecification& specification, const void* data)
	{
		Texture2D texture;

		texture.width = specification.width;
		texture.height = specification.height;
		texture.textureFormat = specification.format;

		uint32_t mipCount = specification.generateMips ? GetMaxMipLevelCount(texture.width, texture.height) : 1;
		uint32_t bytesPerPixel = (texture.textureFormat == wgpu::TextureFormat::RGBA16Float) ? 8 : 4;
		wgpu::Extent3D size = { texture.width, texture.height, 1 };

		wgpu::TextureDescriptor desc;
		desc.dimension = wgpu::TextureDimension::e2D;
		desc.format = texture.textureFormat;
		desc.mipLevelCount = mipCount;
		desc.sampleCount = 1;
		desc.size = size;
		desc.usage = specification.usage | (specification.generateMips ? wgpu::TextureUsage::StorageBinding : wgpu::TextureUsage::None);
		desc.viewFormatCount = 0;
		desc.viewFormats = nullptr;
		texture.texture = GraphicsContext::GetDevice().CreateTexture(&desc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = "Texture2DView";
		viewDesc.format = texture.textureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::e2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
		texture.textureView = texture.texture.CreateView(&viewDesc);

		if (!data) return texture;

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = texture.texture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout;
		layout.offset = 0;
		layout.bytesPerRow = texture.width * bytesPerPixel;
		layout.rowsPerImage = texture.height;

		GraphicsContext::GetQueue().WriteTexture(&dst, data, texture.width * texture.height * bytesPerPixel, &layout, &size);

		if (specification.generateMips)
		{
			GenerateMipmaps(texture.texture);
		}

		return texture;
	}
}