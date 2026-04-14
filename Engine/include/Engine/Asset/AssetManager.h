#pragma once

#include "Engine/Core/Log.h"
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
		void Initialize(const std::filesystem::path& projectDirectory)
		{
			m_AssetRegistry.Load(projectDirectory / "assetRegistry.yaml");
		}

		void Shutdown()
		{
			// TODO: release all assets properly
			m_AssetRegistry.Clear();
			m_LoadedAssets.clear();
		}

		//TODO: handle runtime hot reloading of assets
		//static void OnEvent(Event& e)
		//{
		//	switch (e.GetEventType())
		//	{
		//	case EventType::FileAdded:
		//	{
		//		auto& event = static_cast<FileAddedEvent&>(e);
		//		LOG_INFO("File added: {}", event.GetPath());
		//		CreateAsset(event.GetPath());
		//		break;
		//	}
		//	case EventType::FileRemoved:
		//	{
		//		auto& event = static_cast<FileRemovedEvent&>(e);
		//		LOG_INFO("File removed: {}", event.GetPath());
		//		//TODO: handle file removal (unload asset, remove from registry, etc...)
		//		break;
		//	}
		//	case EventType::FileModified:
		//	{
		//		auto& event = static_cast<FileModifiedEvent&>(e);
		//		LOG_INFO("File modified: {}", event.GetPath());
		//		//TODO: handle file modification (reload asset, etc...)
		//		break;
		//	}
		//	default:
		//		break;
		//	}
		//}

		template<typename T>
		T* GetAsset(Uuid id)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

			Asset* asset = GetLoadedAsset(id);

			if(asset)
			{
				return static_cast<T*>(asset);
			}

			const Engine::AssetRecord& record = m_AssetRegistry.GetRecord(id);
			if (!record.IsValid())
			{
				LOG_ERROR("Failed to load asset '{}', Record not found in registry", id);
				return nullptr;
			}

			asset = AssetImporter::ImportAsset(record);
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
		std::vector<T*> GetAssetsOfType(const AssetType& type) //TODO: this is not very efficient, we should have a map of type to records in the registry
		{
			auto records = m_AssetRegistry.GetAllRecords();
			std::vector<T*> assets;

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
		T* CreateAsset(const std::filesystem::path& path, Args&&... args)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

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

		Asset* GetLoadedAsset(Uuid id)
		{
			auto it = m_LoadedAssets.find(id);
			if (it != m_LoadedAssets.end())
				return it->second;

			return nullptr;
		}

		template<typename Self>
		AssetRecord& GetAssetRecord(this Self&& self, Uuid id)
		{
			return ((AssetManager&)self).m_AssetRegistry.GetRecord(id);
		}

		AssetType GetAssetType(Uuid id)
		{
			const Engine::AssetRecord& record = m_AssetRegistry.GetRecord(id);
			if (!record.IsValid())
			{
				return AssetType::Unknown;
			}
			return record.type;
		}

		Asset* CreateAsset(const std::filesystem::path& filepath)
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

		inline std::filesystem::path GetAssetPath(Uuid id) // returns relative path to the assets directory
		{
			return m_AssetRegistry.GetPath(id);
		}

		inline std::string GetAssetName(Uuid id)
		{
			return m_AssetRegistry.GetPath(id).stem().string();
		}

		std::vector<const AssetRecord*> GetAllAssetRecords()
		{
			return m_AssetRegistry.GetAllRecords();
		}

		std::vector<const AssetRecord*> GetAllAssetRecordsOfType(AssetType type)
		{
			std::vector<const AssetRecord*> allRecords = m_AssetRegistry.GetAllRecords();
			std::vector<const AssetRecord*> records;

			for (auto record : allRecords)
			{
				if (record->type != type) continue;
				records.emplace_back(record);
			}

			return records;
		}

	private:
		AssetRegistry m_AssetRegistry;
		std::unordered_map<Uuid, Asset*> m_LoadedAssets;
	};

	extern ENGINE_API AssetManager* g_AssetManager;
}