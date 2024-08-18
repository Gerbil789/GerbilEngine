#pragma once

#include "Engine/Core/Asset.h"
#include <type_traits>

namespace Engine 
{
    class AssetManager
    {
    public:
        template <typename T>
        static void RegisterFactory(std::unique_ptr<IAssetFactory> factory) {
            factories[typeid(T).name()] = std::move(factory);
        }

        template <typename T>
        static Ref<T> LoadAsset(const std::string& filePath) {
            // Check if asset is already loaded
            auto it_a = assets.find(filePath);
            if (it_a != assets.end())
            {
                return std::dynamic_pointer_cast<T>(it_a->second);
            }

            // Check if factory exists
            auto it_f = factories.find(typeid(T).name());
            if (it_f != factories.end()) {
                auto factory = std::dynamic_pointer_cast<IAssetFactory>(it_f->second);
				auto asset = factory->Create(filePath);
            
				if (asset) {
					assets[filePath] = asset;
                    ENGINE_LOG_INFO("Loaded asset '{0}'", filePath);
					return std::dynamic_pointer_cast<T>(asset);
				}
            }
            else 
            {
                ENGINE_LOG_ERROR("Factory for asset '{0}' not found", filePath);
            }

            ENGINE_LOG_ERROR("Failed to load asset '{0}'", filePath);
            return nullptr;
        }

    private:
        static std::unordered_map<std::string, Ref<IAssetFactory>> factories;
        static std::unordered_map<std::string, Ref<Asset>> assets;
    };
}