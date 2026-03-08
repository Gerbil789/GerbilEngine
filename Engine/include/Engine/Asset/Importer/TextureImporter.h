#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Texture2D;
	class CubeMapTexture;
	struct AssetRecord;

	class ENGINE_API TextureImporter
	{
	public:
		static Texture2D* ImportTexture2D(const AssetRecord& record);
		static Texture2D* LoadTexture2D(const std::filesystem::path& path);

		static CubeMapTexture* ImportCubeMapTexture(const AssetRecord& record);
		static CubeMapTexture* LoadCubeMapTexture(const std::filesystem::path& path);
	};
}