#include "enginepch.h"
#include "TextureImporter.h"
#include "Engine/Core/Project.h"
#include <stb_image.h>

namespace Engine
{
	Ref<Texture2D> TextureImporter::ImportTexture2D(UUID id, const AssetMetadata& metadata)
	{
		return LoadTexture2D(Project::GetAssetsDirectory() / metadata.path);
	}

	Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;
		unsigned char* pixelData = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!pixelData)
		{
			LOG_ERROR("Failed to load texture at: {0}", path);
			return nullptr;
		}

		TextureSpecification spec;
		spec.width = static_cast<uint32_t>(width);
		spec.height = static_cast<uint32_t>(height);
		spec.format = wgpu::TextureFormat::RGBA8Unorm;

		Ref<Texture2D> texture = CreateRef<Texture2D>(spec, pixelData);
		stbi_image_free(pixelData);
		return texture;
	}
}