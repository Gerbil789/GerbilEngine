#pragma once

#include "Engine/Core/Asset.h"
#include <type_traits>
#include <any>

namespace Engine
{
	class AssetManager
	{
	public:
		template <typename T>
		static Ref<T> GetAsset(const std::filesystem::path& path, const std::any& data = std::any())
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

			auto factory = std::dynamic_pointer_cast<IAssetFactory>(factories.find(typeid(T).name())->second);
			auto asset = factory->Load(path, data);

			if (asset == nullptr)
			{
				LOG_ERROR("Failed to load asset '{0}'", path);
				return nullptr;
			}

			assets[path] = asset;
			LOG_TRACE("Loaded asset '{0}'", path);
			return std::dynamic_pointer_cast<T>(asset);
		}

		template <typename T>
		static Ref<T> CreateAsset(const std::filesystem::path& path)
		{
			// Check if asset is already loaded
			auto it_a = assets.find(path);
			if (it_a != assets.end())
			{
				LOG_WARNING("Asset '{0}' already exists", path);
				return std::dynamic_pointer_cast<T>(it_a->second);
			}

			// Check if factory exists
			auto it_f = factories.find(typeid(T).name());
			if (it_f == factories.end())
			{
				LOG_ERROR("Factory for asset '{0}' not found", path);
				return nullptr;
			}

			auto factory = std::dynamic_pointer_cast<IAssetFactory>(it_f->second);
			auto asset = factory->Create(path);

			if (asset)
			{
				assets[path] = asset;
				LOG_TRACE("Created new asset '{0}'", path);
				return std::dynamic_pointer_cast<T>(asset);
			}

			LOG_ERROR("Failed to create asset '{0}'", path);
			return nullptr;
		}


		//TODO: test if this wokrs correctly when loading/unloding scenes
		// use this to unload asset, for example when loading a new scene
		static void AssetManager::UnloadUnusedAssets() {
			for (auto it = assets.begin(); it != assets.end(); )
			{
				if (it->second.use_count() == 1) // Reference count is 1, meaning only AssetManager has a reference to it
				{ 
					LOG_TRACE("Unloaded asset '{0}'", it->first);
					it = assets.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		static std::vector<Ref<Asset>> GetLoadedAssets() 
		{
			std::vector<Ref<Asset>> loadedAssets(assets.size());
			for (auto& asset : assets)
			{
				loadedAssets.emplace_back(asset.second);
			}
			return loadedAssets;
		}

	private:
		static std::unordered_map<std::string, Ref<IAssetFactory>> factories;
		static std::unordered_map<std::filesystem::path, Ref<Asset>> assets;

	};
}