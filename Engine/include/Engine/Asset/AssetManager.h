#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Asset/AssetType.h"
#include <filesystem>

namespace Engine { class AssetRegistry; }

namespace Engine::AssetManager
{
	ENGINE_API void Initialize(const std::filesystem::path& projectDirectory);
	ENGINE_API AssetRegistry& GetAssetRegistry();

	ENGINE_API bool Exists(Uuid id);

	template<typename T>
	ENGINE_API T& GetAsset(Uuid id);
	ENGINE_API AssetType GetAssetType(Uuid id);
	ENGINE_API const std::filesystem::path& GetAssetPath(Uuid id);
	ENGINE_API std::vector<Uuid> GetAssetsOfType(AssetType type);

	template<typename T>
	ENGINE_API T& CreateAsset(const std::filesystem::path& path);

	ENGINE_API void MarkAssetDirty(Uuid id);
	ENGINE_API void SaveDirtyAssets();
}