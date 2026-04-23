#include "enginepch.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include <stb_image.h>
#include <glm/gtc/packing.hpp>

namespace Engine
{
	namespace
	{
		struct ImageLoadResult
		{
			void* stbiData = nullptr;
			std::vector<uint16_t> halfFloatData;

			int width = 0;
			int height = 0;
			wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm;

			const void* GetData() const
			{
				return halfFloatData.empty() ? stbiData : halfFloatData.data();
			}

			~ImageLoadResult()
			{
				if (stbiData) stbi_image_free(stbiData);
			}

			bool Load(const std::filesystem::path& path)
			{
				if (!std::filesystem::exists(path))
				{
					LOG_ERROR("Texture path does not exist: {}", path);
					return false;
				}

				std::string pathStr = path.string();
				int channels = 0;

				if (stbi_is_hdr(pathStr.c_str()))
				{
					float* data32 = stbi_loadf(pathStr.c_str(), &width, &height, &channels, STBI_rgb_alpha);
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
					stbiData = stbi_load(pathStr.c_str(), &width, &height, &channels, STBI_rgb_alpha);
					format = wgpu::TextureFormat::RGBA8Unorm;
				}

				if (!stbiData)
				{
					LOG_ERROR("Failed to load texture at: {}", path);
					LOG_ERROR("stb_image error: {}", stbi_failure_reason());
					return false;
				}

				return true;
			}
		};
	}

	Texture2D* TextureImporter::ImportTexture2D(const std::filesystem::path& path)
	{
		return LoadTexture2D(path);
	}

	Texture2D* TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		ImageLoadResult image;
		if (!image.Load(path)) return nullptr;

		TextureSpecification spec;
		spec.width = static_cast<uint32_t>(image.width);
		spec.height = static_cast<uint32_t>(image.height);
		spec.format = image.format;

		return new Texture2D(spec, image.GetData());
	}

	TextureCube* TextureImporter::ImportCubeTexture(const std::filesystem::path& path)
	{
		return LoadCubeTexture(path);
	}

	TextureCube* TextureImporter::LoadCubeTexture(const std::filesystem::path& path)
	{
		ImageLoadResult image;
		if (!image.Load(path)) return nullptr;

		TextureSpecification spec;
		spec.width = static_cast<uint32_t>(image.width);
		spec.height = static_cast<uint32_t>(image.height);
		spec.format = image.format;

		return new TextureCube(spec, image.GetData());
	}
}