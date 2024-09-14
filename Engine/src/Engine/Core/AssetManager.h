#pragma once

#include "Engine/Core/Asset.h"
#include <type_traits>

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
        static Ref<T> GetAsset(const std::string& filePath) {
            // Check if asset is already loaded
            auto it_a = assets.find(filePath);
            if (it_a != assets.end())
            {
                return std::dynamic_pointer_cast<T>(it_a->second);
            }
            return LoadAsset<T>(filePath);
        }


        // use this only if you want to explicitly pre-load the asset (make sure it is not already loaded)
        template <typename T>
        static Ref<T> LoadAsset(const std::string& filePath) {
            // Check if factory exists
            auto it_f = factories.find(typeid(T).name());
            if (it_f == factories.end()) 
            {
                ENGINE_LOG_ERROR("Factory for asset '{0}' not found", filePath);
                return nullptr;
            }

            auto factory = std::dynamic_pointer_cast<IAssetFactory>(it_f->second);
            auto asset = factory->Load(filePath);

            if (asset) 
            {
                assets[filePath] = asset;
                ENGINE_LOG_TRACE("Loaded asset '{0}'", filePath);
                return std::dynamic_pointer_cast<T>(asset);
            }

            ENGINE_LOG_ERROR("Failed to load asset '{0}'", filePath);
            return nullptr;
        }


        // use this only if you want to create new asset
        template <typename T>
        static Ref<T> CreateAsset(const std::string& filePath) 
        {
            // Check if asset is already loaded
			auto it_a = assets.find(filePath);
			if (it_a != assets.end())
			{
                ENGINE_LOG_WARNING("Asset '{0}' already exists", filePath);
                return std::dynamic_pointer_cast<T>(it_a->second);
			}

			// Check if factory exists
			auto it_f = factories.find(typeid(T).name());
			if (it_f == factories.end()) 
            {
				ENGINE_LOG_ERROR("Factory for asset '{0}' not found", filePath);
				return nullptr;
			}

			auto factory = std::dynamic_pointer_cast<IAssetFactory>(it_f->second);
			auto asset = factory->Create(filePath);

			if (asset) 
            {
				assets[filePath] = asset;
                ENGINE_LOG_TRACE("Created new asset '{0}'", filePath);
				return std::dynamic_pointer_cast<T>(asset);
			}

			ENGINE_LOG_ERROR("Failed to create asset '{0}'", filePath);
			return nullptr;	
		}

        // use this to unload asset, for example when loading a new scene
        static void AssetManager::UnloadUnusedAssets() {
            for (auto it = assets.begin(); it != assets.end(); ) {
                if (it->second.use_count() == 1) { // Reference count is 1, meaning only AssetManager has a reference to it
                    ENGINE_LOG_TRACE("Unloaded asset '{0}'", it->first);
                    it = assets.erase(it);  
                }
                else {
                    ++it;  
                }
            }
        }

    private:
        static std::unordered_map<std::string, Ref<IAssetFactory>> factories;
        static std::unordered_map<std::string, Ref<Asset>> assets;
    };
}