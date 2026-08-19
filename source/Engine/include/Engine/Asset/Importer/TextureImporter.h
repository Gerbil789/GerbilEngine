#pragma once

#include "Engine/Core/API.h"
#include <filesystem>
#include <optional>

namespace Engine
{
	class Texture2DAsset;

	class ENGINE_API TextureImporter
	{
	public:
		static std::optional<Texture2DAsset> LoadTexture2D(const std::filesystem::path& path);
	};
}