#include "enginepch.h"
#include "EditorAssetManager.h"
#include "Engine/Core/Project.h"
#include "Engine/Event/FileEvent.h"

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

	const AssetMetadata* EditorAssetManager::GetAssetMetadata(UUID id) const
	{
		return m_AssetRegistry.GetMetadata(id);

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

	void EditorAssetManager::OnEvent(Event& e)
	{
		switch (e.GetEventType())
		{
		case EventType::FileAdded:
		{
			auto& event = static_cast<FileAddedEvent&>(e);
			LOG_INFO("File added: {0}", event.GetPath());
			//ImportAsset(event.GetFilePath());
			break;
		}
		case EventType::FileRemoved:
		{
			auto& event = static_cast<FileRemovedEvent&>(e);
			LOG_INFO("File removed: {0}", event.GetPath());
			break;
		}
		case EventType::FileModified:
		{
			auto& event = static_cast<FileModifiedEvent&>(e);
			LOG_INFO("File modified: {0}", event.GetPath());
			break;
		}
		default:
		{
			LOG_WARNING("Unhandled event type in EditorAssetManager: {0}", (int)e.GetEventType());
			break;
		}

		}
	}
}