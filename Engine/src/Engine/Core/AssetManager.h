#pragma once

#include "Engine/Core/Asset.h"

namespace Engine 
{
    class AssetManager
    {
    public:
        template<typename T>
        static Ref<T> GetAsset(const std::string& filePath)
        {
            auto it = Assets.find(filePath);
            if (it != Assets.end()) 
            {
                return std::dynamic_pointer_cast<T>(it->second);
            }
            else 
            {
                // Asset not found, load it
                Ref<T> asset = Asset::Create<T>(filePath);
                Assets[filePath] = asset;
                return asset;
            }
        }

    private:
        static std::unordered_map<std::string, Ref<Asset>> Assets;
    };
}