#pragma once

#include "Engine/Asset/AssetType.h"
#include "Engine/Asset/AssetHandle.h"
#include <filesystem>

namespace Engine { class AssetRegistry; }

namespace Engine::AssetManager
{
	ENGINE_API void Initialize(const std::filesystem::path& projectDirectory);
	ENGINE_API AssetRegistry& GetAssetRegistry();

	ENGINE_API bool Exists(Uuid id);

	template<typename Asset>
	ENGINE_API Asset& GetAsset(AssetHandle<Asset> handle);

	ENGINE_API AssetType GetAssetType(Uuid id); //TODO: delete?
	ENGINE_API const std::filesystem::path& GetAssetPath(Uuid id);

	template<typename Handle>
	ENGINE_API Handle CreateAsset(const std::filesystem::path& path);

	ENGINE_API void MarkAssetDirty(Uuid id);
	ENGINE_API void SaveDirtyAssets();
}