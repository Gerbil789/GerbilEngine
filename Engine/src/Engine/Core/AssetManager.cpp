#include "enginepch.h"
#include "Engine/Core/AssetManager.h"

namespace Engine
{
	std::unordered_map<std::filesystem::path, Ref<IAssetFactory>> AssetManager::factories;
	std::unordered_map<std::filesystem::path, Ref<Asset>> AssetManager::assets;
}