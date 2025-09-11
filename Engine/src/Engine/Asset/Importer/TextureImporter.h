#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class TextureImporter
	{
	public:
		static Ref<Texture2D> ImportTexture2D(const AssetMetadata& metadata);
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}