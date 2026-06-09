#pragma once

#include "Engine/Core/UUID.h"
#include <filesystem>

namespace Engine { class AssetRegistry; }

namespace Engine::AssetManager
{
	ENGINE_API void Initialize(const std::filesystem::path& projectDirectory);
	ENGINE_API AssetRegistry& GetAssetRegistry(); //TODO: asset registry should be interal to asset manager only, dont expose it

	template<typename T>
	ENGINE_API T& GetAsset(Uuid id);

	template<typename T>
	ENGINE_API T& CreateAsset(const std::filesystem::path& path);

	//TODO: save asset, delete asset, etc.
}