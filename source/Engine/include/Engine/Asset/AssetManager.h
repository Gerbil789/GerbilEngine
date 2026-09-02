#pragma once

#include "Engine/Asset/AssetType.h"
#include "Engine/Asset/AssetHandle.h"
#include <filesystem>

namespace engine { class AssetRegistry; }

namespace engine::AssetManager
{
	void Initialize();
	AssetRegistry& GetAssetRegistry();

	bool Exists(Uuid id);

	template<typename Asset>
	Asset& GetAsset(AssetHandle<Asset> handle);

	AssetType GetAssetType(Uuid id); //TODO: delete?
	const std::filesystem::path& GetAssetPath(Uuid id);

	template<typename Handle>
	Handle CreateAsset(const std::filesystem::path& path);

	void MarkAssetDirty(Uuid id);
	void SaveDirtyAssets();
}