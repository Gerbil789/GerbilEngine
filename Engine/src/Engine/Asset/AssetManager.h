#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"

#include "Engine/Core/UUID.h"

#include "Engine/Asset/Importer/AssetImporter.h"
#include "Engine/Asset/Serializer/AssetSerializer.h"
#include "Engine/Event/Event.h"

#include "Engine/Event/FileEvent.h"

namespace Engine
{
	class AssetManager
	{
	public:
		static void Initialize()
		{
			m_AssetRegistry.Load(Engine::Project::GetProjectDirectory() / "assetRegistry.yaml");
			m_LoadedAssets.clear();
		}

		static bool IsAssetIdValid(UUID id) 
	{
		return m_AssetRegistry.IsUUIDValid(id);
	}

		template<typename T>
		static Ref<T> GetAsset(UUID id)
		{
			if (!IsAssetIdValid(id))
			{
				LOG_ERROR("EditorAssetManager::GetAsset - Failed to load asset '{0}', Invalid UUID", id);
				return nullptr;
			}

			if (IsAssetLoaded(id))
			{
				return std::static_pointer_cast<T>(m_LoadedAssets.at(id));
			}

			auto metadata = m_AssetRegistry.GetMetadata(id);
			if (!metadata)
			{
				LOG_ERROR("EditorAssetManager::GetAsset - Failed to load asset '{0}', Record not found in registry", id);
				return nullptr;
			}

			Ref<Asset> asset = AssetImporter::ImportAsset(*metadata);
			if (!asset)
			{
				LOG_ERROR("EditorAssetManager::GetAsset - asset import failed! '{0}'", id);
				return nullptr;
			}
			asset->id = id;
			m_LoadedAssets[id] = asset;

			auto relPath = m_AssetRegistry.GetRelativePath(id); // ensure path is relative to assets directory
			LOG_TRACE("Loaded asset '{0}', '{1}'", id, relPath);

			return std::static_pointer_cast<T>(asset);
		}

		template<typename T, typename... Args>
		static Ref<T> CreateAsset(const std::filesystem::path& path, Args&&... args)
		{
			auto metadata = m_AssetRegistry.Create(path);
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

			AssetSerializer::SerializeAsset(asset, *metadata);
			LOG_TRACE("Created new asset '{0}', '{1}'", metadata->id, metadata->path);
			return asset;
		}

		static bool IsAssetValid(UUID id)
		{
			return m_AssetRegistry.IsUUIDValid(id);
		}

		static bool IsAssetLoaded(UUID id)
		{
			return m_LoadedAssets.find(id) != m_LoadedAssets.end();
		}

		//get metadata
		static const AssetMetadata* GetAssetMetadata(UUID id)
		{
			return m_AssetRegistry.GetMetadata(id);
		}

		static void OnEvent(Event& e)
		{
			switch (e.GetEventType())
			{
			case EventType::FileAdded:
			{
				auto& event = static_cast<FileAddedEvent&>(e);
				LOG_INFO("File added: {0}", event.GetPath());
				ImportAsset(event.GetPath());
				break;
			}
			case EventType::FileRemoved:
			{
				auto& event = static_cast<FileRemovedEvent&>(e);
				LOG_INFO("File removed: {0}", event.GetPath());
				//TODO: handle file removal (unload asset, remove from registry, etc...)
				break;
			}
			case EventType::FileModified:
			{
				auto& event = static_cast<FileModifiedEvent&>(e);
				LOG_INFO("File modified: {0}", event.GetPath());
				//TODO: handle file modification (reload asset, etc...)
				break;
			}
			default:
			{
				LOG_WARNING("Unhandled event type in EditorAssetManager: {0}", (int)e.GetEventType());
				break;
			}

			}
		}

		static Ref<Asset> ImportAsset(const std::filesystem::path& filepath)
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

		static inline std::filesystem::path GetAssetPath(UUID id) // returns relative path to the assets directory
		{
			auto metadata = m_AssetRegistry.GetMetadata(id);
			if (!metadata)
			{
				LOG_ERROR("EditorAssetManager::GetAssetPath - Failed to get asset path '{0}', Record not found in registry", id);
				return "";
			}

			return metadata->path;
		}


		inline static AssetRegistry m_AssetRegistry;
		inline static std::unordered_map<UUID, Ref<Asset>> m_LoadedAssets;
	};
}