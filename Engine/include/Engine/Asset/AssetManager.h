#pragma once

#include "Engine/Asset/Asset.h"
#include <filesystem>

namespace Engine
{
	class AssetRegistry;
	class Texture2D;
	class Mesh;
	class Material;
	class Shader;
	class AudioClip;
	class Scene;
}

namespace Engine::AssetManager
{
	ENGINE_API void Initialize(const std::filesystem::path& projectDirectory);

	ENGINE_API AssetRegistry& GetAssetRegistry();

	template<typename T>
	T& GetAsset(Uuid id)
	{
		static_assert(std::is_base_of_v<Asset, T>, "ERROR: Requested type does not derive from Engine::Asset!");
		static_assert(sizeof(T) == 0, "ERROR: Asset type is valid, but missing an explicit template specialization!");
		return *(T*)nullptr;
	}
}

namespace Engine
{
	template<> ENGINE_API Texture2D& AssetManager::GetAsset<Texture2D>(Uuid id);
	template<> ENGINE_API Mesh& AssetManager::GetAsset<Mesh>(Uuid id);
	template<> ENGINE_API Shader& AssetManager::GetAsset<Shader>(Uuid id);
	template<> ENGINE_API Material& AssetManager::GetAsset<Material>(Uuid id);
	template<> ENGINE_API AudioClip& AssetManager::GetAsset<AudioClip>(Uuid id);
	template<> ENGINE_API Scene& AssetManager::GetAsset<Scene>(Uuid id);
}


