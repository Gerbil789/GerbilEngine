#pragma once

#include "Engine/Graphics/Texture/Texture2D.h"

namespace engine
{
	class TextureImporter
	{
	public:
		static Texture2D LoadTexture2D(const std::filesystem::path& path);
	};
}