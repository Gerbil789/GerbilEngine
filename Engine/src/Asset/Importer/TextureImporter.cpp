#include "enginepch.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/Texture.h"
#include <stb_image.h>

namespace Engine
{
	Texture2D* TextureImporter::ImportTexture2D(const std::filesystem::path& path)
	{
		return LoadTexture2D(path);
	}

	Texture2D* TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;
		void* data;

		TextureSpecification spec;

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

	CubeMapTexture* TextureImporter::ImportCubeMapTexture(const std::filesystem::path& path)
	{
		return LoadCubeMapTexture(path);
	}

	CubeMapTexture* TextureImporter::LoadCubeMapTexture(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Texture path does not exist: {}", path);
			return nullptr;
		}


		int width = 0, height = 0, channels = 0;
		void* data= nullptr;
		wgpu::TextureFormat format;

		if (stbi_is_hdr(path.string().c_str()))
		{
			data = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			format = wgpu::TextureFormat::RGBA32Float;
		}
		else
		{
			data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			format = wgpu::TextureFormat::RGBA8Unorm;
		}

		if (!data)
		{
			LOG_ERROR("Failed to load texture: {}", path);
			return nullptr;
		}

		TextureSpecification spec;
		spec.width = width;
		spec.height = height;
		spec.format = format;

		CubeMapTexture* cubeMap = new CubeMapTexture(spec, data);

		stbi_image_free(data);

		return cubeMap;
	}
}