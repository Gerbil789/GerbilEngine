#include "enginepch.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Asset/AssetRecord.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Core/Engine.h"
#include <stb_image.h>

namespace Engine
{
	Texture2D* TextureImporter::ImportTexture2D(const AssetRecord& record)
	{
		return LoadTexture2D(Engine::GetAssetsDirectory() / record.path);
	}

	Texture2D* TextureImporter::LoadTexture2D(const std::filesystem::path& path, TextureSpecification spec)
	{
		int width, height, channels;
		void* data;

		if (stbi_is_hdr(path.string().c_str()))
		{
			data = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			spec.format = wgpu::TextureFormat::RGBA32Float;
		}
		else
		{
			data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			spec.format = wgpu::TextureFormat::RGBA8Unorm;
		}

		if (!data)
		{
			LOG_ERROR("Failed to load texture at: {}", path);
			LOG_ERROR("stb_image error: {}", stbi_failure_reason());
			return nullptr;
		}

		spec.width = static_cast<uint32_t>(width);
		spec.height = static_cast<uint32_t>(height);
		Texture2D* texture = new Texture2D(spec, data);
		stbi_image_free(data);
		return texture;
	}

	uint16_t FloatToHalf(float f)
	{
		uint32_t x = *(uint32_t*)&f;

		uint32_t sign = (x >> 16) & 0x8000;
		uint32_t mantissa = x & 0x7fffff;
		int exp = ((x >> 23) & 0xff) - 127 + 15;

		if (exp <= 0)
			return (uint16_t)sign;
		if (exp >= 31)
			return (uint16_t)(sign | 0x7c00);

		return (uint16_t)(sign | (exp << 10) | (mantissa >> 13));
	}

  wgpu::TextureView TextureImporter::LoadTexture2DWithMipMaps(const std::vector<std::filesystem::path>& paths)
  {
    struct MipData
    {
      void* data;
      int width;
      int height;
    };

    std::vector<MipData> mips;
    mips.reserve(paths.size());

    int channels = 0;
    wgpu::TextureFormat format;

    for (size_t i = 0; i < paths.size(); i++)
    {
			// check if path is valid
			if (!std::filesystem::exists(paths[i]))
			{
				LOG_ERROR("Texture path does not exist: {}", paths[i]);
				return nullptr;
			}

      int width, height;
      void* data = nullptr;

      if (stbi_is_hdr(paths[i].string().c_str()))
      {
        data = stbi_loadf(paths[i].string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        format = wgpu::TextureFormat::RGBA16Float;
      }
      else
      {
        data = stbi_load(paths[i].string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        format = wgpu::TextureFormat::RGBA8Unorm;
      }

      if (!data)
      {
        LOG_ERROR("Failed to load texture: {}", paths[i]);
        return nullptr;
      }

      if (i > 0)
      {
        if (width != mips[i - 1].width / 2 || height != mips[i - 1].height / 2)
        {
          LOG_ERROR("Invalid mip chain at level {}", i);
          return nullptr;
        }
      }

      mips.push_back({ data, width, height });
    }

    uint32_t mipCount = (uint32_t)mips.size();
    uint32_t baseWidth = mips[0].width;
    uint32_t baseHeight = mips[0].height;

		uint32_t bytesPerPixel = 4; 
		if (format == wgpu::TextureFormat::RGBA16Float) bytesPerPixel = 8;
		if (format == wgpu::TextureFormat::RGBA32Float) bytesPerPixel = 16;

    wgpu::TextureDescriptor desc;
		desc.label = { "Texture2DWithMipMaps", WGPU_STRLEN };
    desc.dimension = wgpu::TextureDimension::_2D;
    desc.size = { baseWidth, baseHeight, 1 };
    desc.mipLevelCount = mipCount;
    desc.sampleCount = 1;
    desc.format = format;
    desc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		desc.viewFormatCount = 0;
		desc.viewFormats = nullptr;

    wgpu::Texture texture = GraphicsContext::GetDevice().createTexture(desc);

    for (uint32_t mip = 0; mip < mipCount; mip++)
    {
			const auto& m = mips[mip];

			wgpu::TexelCopyTextureInfo dst;
			dst.texture = texture;
			dst.mipLevel = mip;
			dst.origin = { 0, 0, 0 };
			dst.aspect = wgpu::TextureAspect::All;

			wgpu::TexelCopyBufferLayout layout;
			layout.offset = 0;
			layout.bytesPerRow = m.width * bytesPerPixel;
			layout.rowsPerImage = m.height;


			wgpu::Extent3D size = { (uint32_t)m.width, (uint32_t)m.height, 1 };

			if (format == wgpu::TextureFormat::RGBA16Float)
			{
				float* src = (float*)m.data;
				size_t pixelCount = m.width * m.height * 4;

				std::vector<uint16_t> converted(pixelCount);

				for (size_t i = 0; i < pixelCount; i++)
					converted[i] = FloatToHalf(src[i]);

				layout.bytesPerRow = m.width * 8;

				GraphicsContext::GetQueue().writeTexture(
					dst,
					converted.data(),
					converted.size() * sizeof(uint16_t),
					layout,
					size
				);
			}
			else
			{
				// your existing path
				GraphicsContext::GetQueue().writeTexture(dst, m.data, (size_t)(m.width* m.height* bytesPerPixel), layout, size);
			}


			stbi_image_free(m.data);
    }


    // create view 
    wgpu::TextureViewDescriptor viewDesc;
    viewDesc.dimension = wgpu::TextureViewDimension::_2D;
    viewDesc.format = format;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = mipCount;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
    viewDesc.aspect = wgpu::TextureAspect::All;

    wgpu::TextureView view = texture.createView(viewDesc);
    return view;
  }

	CubeMapTexture* TextureImporter::ImportCubeMapTexture(const AssetRecord& record)
	{
		return LoadCubeMapTexture(Engine::GetAssetsDirectory() / record.path);
	}

	CubeMapTexture* TextureImporter::LoadCubeMapTexture(const std::filesystem::path& path)
	{
		const std::array<std::string, 6> fileNames = { "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png" };
		std::array<std::vector<uint8_t>, 6> faces;
		int width = 0, height = 0;

		for (size_t i = 0; i < 6; ++i)
		{
			std::filesystem::path facePath = path / fileNames[i];
			int w, h, c;
			unsigned char* data = stbi_load(facePath.string().c_str(), &w, &h, &c, STBI_rgb_alpha);

			if (!data)
			{
				LOG_ERROR("Failed to load cubemap face: {}", facePath);
				return nullptr;
			}

			if (i == 0)
			{
				width = w;
				height = h;
			}
			else
			{
				// check all faces have same dimensions
				if (w != width || h != height)
				{
					LOG_ERROR("Cubemap face {} has different size!", facePath);
					stbi_image_free(data);
					return nullptr;
				}
			}

			faces[i].resize(width * height * 4);
			std::memcpy(faces[i].data(), data, width * height * 4);
			stbi_image_free(data);
		}

		TextureSpecification spec;
		spec.width = width;
		spec.height = height;
		spec.format = wgpu::TextureFormat::RGBA8Unorm;

		std::array<const void*, 6> facePtrs;
		for (int i = 0; i < 6; ++i)
			facePtrs[i] = faces[i].data();

		return new CubeMapTexture(spec, facePtrs);
	}
}