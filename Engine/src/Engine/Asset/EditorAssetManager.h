#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/UUID.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRegistry.h"

namespace Engine
{
	class EditorAssetManager
	{
	public:
		void Initialize();

		Ref<Asset> GetAsset(UUID id);

		template<typename T, typename ... Args>
		Ref<T> CreateAsset(std::filesystem::path path, Args&&... args)
		{
			auto metadata = m_AssetRegistry.Add(path);
			if (!metadata)
			{
				LOG_ERROR("EditorAssetManager::CreateAsset - Failed to add record in registry '{0}'", path);
				return nullptr;
			}

			Ref<T> asset = CreateRef<T>(std::forward<Args>(args)...);
			if (!asset)
			{
				LOG_ERROR("EditorAssetManager::CreateAsset - failed to create asset. '{0}'", path);
				return nullptr;
			}
			asset->id = metadata->id;

			m_LoadedAssets[metadata->id] = asset;
			LOG_TRACE("Created new asset '{0}', '{1}'", metadata->id, metadata->path);
			return asset;
		}

		bool IsAssetIdValid(UUID id) const;
		bool IsAssetLoaded(UUID id) const;

		Ref<Asset> ImportAsset(const std::filesystem::path& filepath);
	private:
		AssetRegistry m_AssetRegistry;
		std::unordered_map<UUID, Ref<Asset>> m_LoadedAssets;
	};
}