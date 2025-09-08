#include "enginepch.h"
#include "EditorAssetManager.h"
#include "Engine/Asset/Importer/AssetImporter.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	void EditorAssetManager::Initialize()
	{
		m_AssetRegistry.Load("AssetRegistry.yaml");
	}

	Ref<Asset> EditorAssetManager::GetAsset(UUID id)
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

		Ref<Asset> asset = AssetImporter::ImportAsset(id, *metadata);
		if (!asset)
		{
			LOG_ERROR("EditorAssetManager::GetAsset - asset import failed! '{0}'", id);
			return nullptr;
		}

		m_LoadedAssets[id] = asset;
		LOG_TRACE("Loaded asset '{0}', '{1}'", id, metadata->path);
		return asset;
	}

	/*Ref<Asset> EditorAssetManager::CreateAsset(std::filesystem::path path)
	{
		auto metadata = m_AssetRegistry.Add(path);
		if (!metadata)
		{
			LOG_ERROR("EditorAssetManager::CreateAsset - Failed to add record in registry '{0}'", path);
			return nullptr;
		}

		Ref<Asset> asset = CreateRef<Asset>();
		if (!asset)
		{
			LOG_ERROR("EditorAssetManager::CreateAsset - failed to create asset. '{0}'", path);
			return nullptr;
		}
		asset->id = metadata->id;

		m_LoadedAssets[metadata->id] = asset;
		LOG_TRACE("Created new asset '{0}', '{1}'", metadata->id, metadata->path);
		return asset;
	}*/


	bool EditorAssetManager::IsAssetIdValid(UUID id) const
	{
		return m_AssetRegistry.IsUUIDValid(id);
	}

	bool EditorAssetManager::IsAssetLoaded(UUID id) const
	{
		return m_LoadedAssets.find(id) != m_LoadedAssets.end();
	}

	Ref<Asset> EditorAssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		auto metadata = m_AssetRegistry.Add(filepath);
		if (!metadata)
		{
			return nullptr;
		}

		if (IsAssetLoaded(metadata->id))
		{
			return m_LoadedAssets.at(metadata->id);
		}

		Ref<Asset> asset = AssetImporter::ImportAsset(metadata->id, *metadata);
		if (!asset)
		{
			LOG_ERROR("EditorAssetManager::ImportAsset - asset import failed! '{0}'", filepath.string());
			return nullptr;
		}

		m_LoadedAssets[asset->id] = asset;
		return asset;
	}
}