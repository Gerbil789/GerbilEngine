#pragma once

#include "Engine/Asset/AssetRecord.h"
#include "Engine/Graphics/Texture.h"

namespace Engine
{
	class TextureImporter
	{
	public:
		static Texture2D* ImportTexture2D(const AssetRecord& record);
		static Texture2D* LoadTexture2D(const std::filesystem::path& path);

		static CubeMapTexture* ImportCubeMapTexture(const AssetRecord& record);
		static CubeMapTexture* LoadCubeMapTexture(const std::filesystem::path& path);
	};
}