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
        static void RegisterFactory(std::unique_ptr<IAssetFactory> factory) 
        {
            factories[typeid(T).name()] = std::move(factory);
        }

        // use this to get asset, if it is not loaded it will be loaded
        template <typename T>
        static Ref<T> GetAsset(const std::filesystem::path& path, const std::any& data = std::any()) {
            // Check if asset is already loaded
            auto it_a = assets.find(path);
            if (it_a != assets.end())
            {
                return std::dynamic_pointer_cast<T>(it_a->second);
            }
            return LoadAsset<T>(path, data);
        }


        // use this only if you want to explicitly pre-load the asset (make sure it is not already loaded)
        template <typename T>
        static Ref<T> LoadAsset(const std::filesystem::path& path, const std::any& data = std::any()) {
            // Check if factory exists
            auto it_f = factories.find(typeid(T).name());
            if (it_f == factories.end()) 
            {
                LOG_ERROR("Factory for asset '{0}' not found", path.string());
                return nullptr;
            }

            auto factory = std::dynamic_pointer_cast<IAssetFactory>(it_f->second);
            auto asset = factory->Load(path, data);

            if (asset) 
            {
                assets[path] = asset;
                LOG_TRACE("Loaded asset '{0}'", path);
                return std::dynamic_pointer_cast<T>(asset);
            }

            LOG_ERROR("Failed to load asset '{0}'", path.string());
            return nullptr;
        }


        // use this only if you want to create new asset
        template <typename T>
        static Ref<T> CreateAsset(const std::filesystem::path& path) 
        {
            // Check if asset is already loaded
			auto it_a = assets.find(path);
			if (it_a != assets.end())
			{
                LOG_WARNING("Asset '{0}' already exists", path.string());
                return std::dynamic_pointer_cast<T>(it_a->second);
			}

			// Check if factory exists
			auto it_f = factories.find(typeid(T).name());
			if (it_f == factories.end()) 
            {
				LOG_ERROR("Factory for asset '{0}' not found", path.string());
				return nullptr;
			}

			auto factory = std::dynamic_pointer_cast<IAssetFactory>(it_f->second);
			auto asset = factory->Create(path);

			if (asset) 
            {
				assets[path] = asset;
                LOG_TRACE("Created new asset '{0}'", path.string());
				return std::dynamic_pointer_cast<T>(asset);
			}

			LOG_ERROR("Failed to create asset '{0}'", path.string());
			return nullptr;	
		}

        //TODO: test if its wokrs correctly when loading/unloding scenes
        // use this to unload asset, for example when loading a new scene
        static void AssetManager::UnloadUnusedAssets() {
            for (auto it = assets.begin(); it != assets.end(); ) {
                if (it->second.use_count() == 1) { // Reference count is 1, meaning only AssetManager has a reference to it
                    LOG_TRACE("Unloaded asset '{0}'", it->first);
                    it = assets.erase(it);  
                }
                else {
                    ++it;  
                }
            }
        }

    private:
        static std::unordered_map<std::filesystem::path, Ref<IAssetFactory>> factories;
        static std::unordered_map<std::filesystem::path, Ref<Asset>> assets;
    };
}