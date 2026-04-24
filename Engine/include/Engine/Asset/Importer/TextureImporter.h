#pragma once

#include "Engine/Core/API.h"
#include <optional>

namespace Engine
{
	class Texture2D;

	class ENGINE_API TextureImporter
	{
	public:
		static std::optional<Texture2D> LoadTexture(const std::filesystem::path& path);
	};
}