#include "enginepch.h"
#include "Engine/Core/AssetManager.h"

namespace Engine
{
	std::unordered_map<std::string, Ref<IAssetFactory>> AssetManager::factories;
	std::unordered_map<std::string, Ref<Asset>> AssetManager::assets;
}