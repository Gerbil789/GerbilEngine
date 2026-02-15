#pragma once

#include "Engine/Core/Log.h"
#include "Engine/Core/Engine.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Asset/Importer/AssetImporter.h"
#include "Engine/Asset/Serializer/AssetSerializer.h"
#include "Engine/Event/Event.h"
#include "Engine/Event/FileEvent.h"

namespace Engine
{
	class ENGINE_API AssetManager
	{
	public:
		static void Initialize()
		{
			m_AssetRegistry.Load(Engine::GetProjectDirectory() / "assetRegistry.yaml");
		}

		static void Shutdown()
		{
			// TODO: release all assets properly
			m_AssetRegistry.Clear();
			m_LoadedAssets.clear();
		}

		static void OnEvent(Event& e)
		{
			switch (e.GetEventType())
			{
			case EventType::FileAdded:
			{
				auto& event = static_cast<FileAddedEvent&>(e);
				LOG_INFO("File added: {}", event.GetPath());
				CreateAsset(event.GetPath());
				break;
			}
			case EventType::FileRemoved:
			{
				auto& event = static_cast<FileRemovedEvent&>(e);
				LOG_INFO("File removed: {}", event.GetPath());
				//TODO: handle file removal (unload asset, remove from registry, etc...)
				break;
			}
			case EventType::FileModified:
			{
				auto& event = static_cast<FileModifiedEvent&>(e);
				LOG_INFO("File modified: {}", event.GetPath());
				//TODO: handle file modification (reload asset, etc...)
				break;
			}
			default:
				break;
			}
		}

		template<typename T>
		static T* GetAsset(Uuid id)
		{
			Asset* asset = GetLoadedAsset(id);

			if (asset)
			{
				return static_cast<T*>(asset);
			}

			auto record = m_AssetRegistry.GetRecord(id);
			if (!record)
			{
				LOG_ERROR("Failed to load asset '{}', Record not found in registry", id);
				return nullptr;
			}

			asset = AssetImporter::ImportAsset(*record);
			if (asset)
			{
				LOG_TRACE("Loaded asset '{}', '{}'", id, m_AssetRegistry.GetRelativePath(id));
				m_LoadedAssets[id] = asset;
				return static_cast<T*>(asset);
			}

			LOG_ERROR("asset import failed! '{}'", id);
			return nullptr;
		}


		template<typename T>
		static std::vector<T*> GetAssetsOfType(const AssetType& type)
		{
			std::vector<T*> assets;
			auto records = m_AssetRegistry.GetAllRecords();

			for (const auto& record : records)
			{
				if (record->type != type)
				{
					continue;
				}

				Asset* asset = GetLoadedAsset(record->id);

				if (!asset)
				{
					asset = AssetImporter::ImportAsset(*record);
					if (!asset)
					{
						LOG_ERROR("asset import failed! '{}'", record->path.string());
						continue;
					}
					m_LoadedAssets[record->id] = asset;
				}

				assets.push_back(static_cast<T*>(asset));
			}
			return assets;
		}

		template<typename T, typename... Args>
		static T* CreateAsset(const std::filesystem::path& path, Args&&... args)
		{
			auto record = m_AssetRegistry.Create(path);
			if (!record)
			{
				LOG_ERROR("Failed to add record in registry '{}'", path);
				return nullptr;
			}

			Asset* asset = new T(std::forward<Args>(args)...);
			if (!asset)
			{
				LOG_ERROR("failed to create asset. '{}'", path);
				return nullptr;
			}

			asset->id = record->id;
			m_LoadedAssets[record->id] = asset;

			AssetSerializer::SerializeAsset(asset, *record);
			LOG_TRACE("Created new asset '{}', '{}'", record->id, record->path);
			return static_cast<T*>(asset);
		}

		static Asset* GetLoadedAsset(Uuid id)
		{
			auto it = m_LoadedAssets.find(id);
			if (it != m_LoadedAssets.end())
				return it->second;

			return nullptr;
		}

		static const AssetRecord* GetAssetRecord(Uuid id)
		{
			return m_AssetRegistry.GetRecord(id);
		}

		static AssetType GetAssetType(Uuid id)
		{
			auto record = m_AssetRegistry.GetRecord(id);
			if (!record)
			{
				return AssetType::Unknown;
			}
			return record->type;
		}

		static Asset* CreateAsset(const std::filesystem::path& filepath)
		{
			auto record = m_AssetRegistry.Create(filepath);
			if (!record)
			{
				LOG_ERROR("Failed to add record in registry '{}'", filepath);
				return nullptr;
			}

			if (Asset* asset = GetLoadedAsset(record->id))
			{
				return asset;
			}

			Asset* asset = AssetImporter::ImportAsset(*record);
			if (!asset)
			{
				LOG_ERROR("Asset import failed! '{}'", filepath);
				return nullptr;
			}

			m_LoadedAssets[asset->id] = asset;

			return asset;
		}

		static inline std::filesystem::path GetAssetPath(Uuid id) // returns relative path to the assets directory
		{
			return m_AssetRegistry.GetPath(id);
		}

		static inline std::string GetAssetName(Uuid id)
		{
			return m_AssetRegistry.GetPath(id).stem().string();
		}

		static std::vector<const AssetRecord*> GetAllAssetRecords()
		{
			return m_AssetRegistry.GetAllRecords();
		}

	private:
		inline static AssetRegistry m_AssetRegistry;
		inline static std::unordered_map<Uuid, Asset*> m_LoadedAssets;
	};
}