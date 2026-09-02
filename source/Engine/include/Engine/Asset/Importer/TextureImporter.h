#pragma once

#include <filesystem>
#include <optional>

namespace engine
{
	class Texture2DAsset;

	class TextureImporter
	{
	public:
		static std::optional<Texture2DAsset> LoadTexture2D(const std::filesystem::path& path);
	};
}