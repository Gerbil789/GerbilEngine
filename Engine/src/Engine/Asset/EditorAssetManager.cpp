#include "enginepch.h"
#include "EditorAssetManager.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	void EditorAssetManager::Initialize()
	{
		m_AssetRegistry.Load(Engine::Project::GetProjectDirectory() / "assetRegistry.yaml");
	}

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
		auto metadata = m_AssetRegistry.Create(filepath);
		if (!metadata)
		{
			return nullptr;
		}

		if (IsAssetLoaded(metadata->id))
		{
			return m_LoadedAssets.at(metadata->id);
		}

		Ref<Asset> asset = AssetImporter::ImportAsset(*metadata);
		if (!asset)
		{
			LOG_ERROR("EditorAssetManager::ImportAsset - asset import failed! '{0}'", filepath.string());
			return nullptr;
		}

		m_LoadedAssets[asset->id] = asset;
		return asset;
	}
}