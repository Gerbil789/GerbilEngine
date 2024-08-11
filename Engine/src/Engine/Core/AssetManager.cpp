#include "enginepch.h"
#include "Engine/Core/AssetManager.h"

namespace Engine
{
	std::unordered_map<std::string, Ref<Asset>> AssetManager::Assets;
}