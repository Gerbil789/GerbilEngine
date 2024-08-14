#pragma once

#include "Engine/Core/Asset.h"
#include <type_traits>

namespace Engine 
{
    template <typename T>
    class has_create {
    private:
        template <typename U>
        static auto test(int) -> decltype(U::Create(std::declval<std::string>()), std::true_type());

        template <typename>
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    class AssetManager
    {
    public:
        // Classes with factory methods
        template<typename T, typename std::enable_if<has_create<T>::value, int>::type = 0>
        static Ref<T> LoadAsset(const std::string& filePath)
        {
            auto it = Assets.find(filePath);
            if (it != Assets.end()) 
            {
                return std::dynamic_pointer_cast<T>(it->second);
            }

            Ref<T> asset = T::Create(filePath);
            if (asset) 
            {
                Assets[filePath] = asset;
                return asset;
            }

            ENGINE_LOG_ERROR("Failed to load asset '{0}'", filePath);
            return nullptr;
        }

        // This version is for types that do NOT have a Create method
        template<typename T, typename std::enable_if<!has_create<T>::value, int>::type = 0>
        static Ref<T> LoadAsset(const std::string& filePath)
        {
            auto it = Assets.find(filePath);
            if (it != Assets.end())
            {
                return std::dynamic_pointer_cast<T>(it->second);
            }

            Ref<T> asset = CreateRef<T>(); 
            asset->Load(filePath);
            Assets[filePath] = asset;
            return asset;
        }


      /*  static void UnloadAsset(const std::string& filePath) 
        {
            auto it = Assets.find(filePath);
            if (it != Assets.end()) 
            {
                it->second->Unload();
                Assets.erase(it);
            }
        }

        static void UnloadAllAssets() 
        {
            for (auto& pair : Assets) 
            {
                pair.second->Unload();
            }
            Assets.clear();
        }*/

    private:
        static std::unordered_map<std::string, Ref<Asset>> Assets;

       
    };
}