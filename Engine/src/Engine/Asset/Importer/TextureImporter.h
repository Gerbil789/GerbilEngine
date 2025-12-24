#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Graphics/Texture.h"

namespace Engine
{
	class TextureImporter
	{
	public:
		static Texture2D* ImportTexture2D(const AssetMetadata& metadata);
		static Texture2D* LoadTexture2D(const std::filesystem::path& path);

		static CubeMapTexture* ImportCubeMapTexture(const AssetMetadata& metadata);
		static CubeMapTexture* LoadCubeMapTexture(const std::filesystem::path& path);
	};
}