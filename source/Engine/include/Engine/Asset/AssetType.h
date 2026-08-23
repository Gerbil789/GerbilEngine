#pragma once

#include <string_view>

namespace Engine
{
	enum class AssetType { Unknown = 0, Texture, Mesh, Shader, Material, Audio, Scene, Script };

	AssetType GetAssetTypeFromExtension(std::string_view extension);

	std::string_view AssetTypeToString(AssetType type);
}