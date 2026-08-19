#pragma once

#include "Engine/Core/API.h"
#include <string_view>


//TODO: delete this?

namespace Engine
{
	enum class AssetType { Unknown = 0, Texture, Mesh, Shader, Material, Audio, Scene, Script };

	AssetType GetAssetTypeFromExtension(std::string_view extension);

	ENGINE_API std::string_view AssetTypeToString(AssetType type);
}