#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Texture2D;
	class TextureCube;

	class ENGINE_API TextureImporter
	{
	public:
		static Texture2D* ImportTexture2D(const std::filesystem::path& path);
		static Texture2D* LoadTexture2D(const std::filesystem::path& path);

		static TextureCube* ImportCubeTexture(const std::filesystem::path& path);
		static TextureCube* LoadCubeTexture(const std::filesystem::path& path);
	};
}