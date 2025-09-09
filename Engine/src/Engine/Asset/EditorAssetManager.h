#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/UUID.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Asset/Importer/AssetImporter.h"

namespace Engine
{
	class EditorAssetManager
	{
	public:
		void Initialize();

		template<typename T>
		Ref<T> GetAsset(UUID id)
		{
			if (!IsAssetIdValid(id))
			{
				LOG_ERROR("EditorAssetManager::GetAsset - Failed to load asset '{0}', Invalid UUID", id);
				return nullptr;
			}

			if (IsAssetLoaded(id))
			{
				return m_LoadedAssets.at(id);
			}

			auto metadata = m_AssetRegistry.GetMetadata(id);
			if (!metadata)
			{
				LOG_ERROR("EditorAssetManager::GetAsset - Failed to load asset '{0}', Record not found in registry", id);
				return nullptr;
			}

			Ref<T> asset = AssetImporter::ImportAsset(id, *metadata);
			if (!asset)
			{
				LOG_ERROR("EditorAssetManager::GetAsset - asset import failed! '{0}'", id);
				return nullptr;
			}

			m_LoadedAssets[id] = asset;
			LOG_TRACE("Loaded asset '{0}', '{1}'", id, metadata->path);
			return asset;
		}

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