#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Texture2D;
	class CubeMapTexture;

	class ENGINE_API TextureImporter
	{
	public:
		static Texture2D* ImportTexture2D(const std::filesystem::path& path);
		static Texture2D* LoadTexture2D(const std::filesystem::path& path);

		static CubeMapTexture* ImportCubeMapTexture(const std::filesystem::path& path);
		static CubeMapTexture* LoadCubeMapTexture(const std::filesystem::path& path);
	};
}