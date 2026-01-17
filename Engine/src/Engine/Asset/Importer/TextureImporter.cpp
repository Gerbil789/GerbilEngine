#include "enginepch.h"
#include "TextureImporter.h"
#include "Engine/Core/Project.h"
#include <stb_image.h>

namespace Engine
{
	Texture2D* TextureImporter::ImportTexture2D(const AssetRecord& record)
	{
		return LoadTexture2D(Project::GetAssetsDirectory() / record.path);
	}

	Texture2D* TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;
		unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!data)
		{
			LOG_ERROR("Failed to load texture at: {}", path);
			return nullptr;
		}

		TextureSpecification spec;
		spec.width = static_cast<uint32_t>(width);
		spec.height = static_cast<uint32_t>(height);
		spec.format = wgpu::TextureFormat::RGBA8Unorm;

		Texture2D* texture = new Texture2D(spec, data);
		stbi_image_free(data);
		return texture;
	}

	CubeMapTexture* TextureImporter::ImportCubeMapTexture(const AssetRecord& record)
	{
		return LoadCubeMapTexture(Project::GetAssetsDirectory() / record.path);
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