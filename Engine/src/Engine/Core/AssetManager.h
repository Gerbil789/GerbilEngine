#pragma once

#include "Engine/Core/Asset.h"

namespace Engine::AssetManager
{
	using AssetLoaderFn = std::function<Ref<Asset>(const std::filesystem::path&)>;
	using AssetCreatorFn = std::function<Ref<Asset>(const std::filesystem::path&)>;

	inline std::unordered_map<std::filesystem::path, Ref<Asset>> assets;
	inline std::unordered_map<std::string, AssetLoaderFn> assetLoaders;
	inline std::unordered_map<std::string, AssetCreatorFn> assetCreators;

	void Initialize();

	template<typename T>
	void RegisterAssetType(AssetLoaderFn loader, AssetCreatorFn creator)
	{
		assetLoaders[typeid(T).name()] = loader;
		assetCreators[typeid(T).name()] = creator;
	}

	template <typename T>
	Ref<T> Get(const std::filesystem::path& path)
	{
		auto it_a = assets.find(path);
		if (it_a != assets.end())
		{
			return std::dynamic_pointer_cast<T>(it_a->second);
		}

		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Asset '{0}' not found", path);
			return nullptr;
		}

		auto it_l = assetLoaders.find(typeid(T).name());
		if (it_l == assetLoaders.end())
		{
			LOG_ERROR("No loader registered for type '{0}'", typeid(T).name());
			return nullptr;
		}

		auto asset = it_l->second(path);
		if (!asset)
		{
			LOG_ERROR("Failed to load asset '{0}'", path);
			return nullptr;
		}

		assets[path] = asset;
		LOG_TRACE("Loaded asset '{0}'", path);
		return std::dynamic_pointer_cast<T>(asset);
	}

	template <typename T>
	Ref<T> Create(const std::filesystem::path& path)
	{
		auto it_a = assets.find(path);
		if (it_a != assets.end())
		{
			LOG_WARNING("Asset '{0}' already exists", path);
			return std::dynamic_pointer_cast<T>(it_a->second);
		}

		auto it_c = assetCreators.find(typeid(T).name());
		if (it_c == assetCreators.end())
		{
			LOG_ERROR("No creator registered for type '{0}'", typeid(T).name());
			return nullptr;
		}

		auto asset = it_c->second(path);
		if (!asset)
		{
			LOG_ERROR("Failed to create asset '{0}'", path);
			return nullptr;
		}

		assets[path] = asset;
		LOG_TRACE("Created asset '{0}'", path);
		return std::dynamic_pointer_cast<T>(asset);
	}

	//TODO: Unload assets
}