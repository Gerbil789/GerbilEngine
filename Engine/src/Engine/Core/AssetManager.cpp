#include "enginepch.h"
#include "AssetManager.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Mesh.h"

namespace Engine
{
	std::unordered_map<std::string, Ref<IAssetFactory>> AssetManager::factories
	{
		{ typeid(Texture2D).name(),std::make_unique<Texture2DFactory>() },
		{ typeid(Material).name(),std::make_unique<MaterialFactory>() },
		{ typeid(Scene).name(),std::make_unique<SceneFactory>() },
		{ typeid(Mesh).name(),std::make_unique<MeshFactory>() }
	};
	
	std::unordered_map<std::filesystem::path, Ref<Asset>> AssetManager::assets;
}