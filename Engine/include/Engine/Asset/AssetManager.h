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
	class AssetManager
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
			Asset* asset = nullptr;

			if (IsAssetLoaded(id))
			{
				asset = m_LoadedAssets.at(id);
			}
			else
			{
				auto metadata = m_AssetRegistry.GetRecord(id);
				if (!metadata)
				{
					LOG_ERROR("AssetManager::GetAsset - Failed to load asset '{}', Record not found in registry", id);
					return nullptr;
				}

				asset = AssetImporter::ImportAsset(*metadata);
				if (!asset)
				{
					LOG_ERROR("AssetManager::GetAsset - asset import failed! '{}'", id);
					return nullptr;
				}
				m_LoadedAssets[id] = asset;

				LOG_TRACE("Loaded asset '{}', '{}'", id, m_AssetRegistry.GetRelativePath(id));

			}
			auto ret = static_cast<T*>(asset);
			return ret;
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

				Asset* asset = nullptr;
				if (IsAssetLoaded(record->id))
				{
					asset = m_LoadedAssets.at(record->id);
				}
				else
				{
					asset = AssetImporter::ImportAsset(*record);
					if (!asset)
					{
						LOG_ERROR("AssetManager::GetAssetsOfType - asset import failed! '{}'", record->path.string());
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
			auto metadata = m_AssetRegistry.Create(path);
			if (!metadata)
			{
				LOG_ERROR("AssetManager::CreateAsset - Failed to add record in registry '{}'", path);
				return nullptr;
			}

			Asset* asset = new T(std::forward<Args>(args)...);
			if (!asset)
			{
				LOG_ERROR("AssetManager::CreateAsset - failed to create asset. '{}'", path);
				return nullptr;
			}

			asset->id = metadata->id;
			m_LoadedAssets[metadata->id] = asset;

			AssetSerializer::SerializeAsset(asset, *metadata);
			LOG_TRACE("Created new asset '{}', '{}'", metadata->id, metadata->path);
			return static_cast<T*>(asset);
		}

		static bool IsAssetLoaded(Uuid id)
		{
			return m_LoadedAssets.find(id) != m_LoadedAssets.end();
		}

		static const AssetRecord* GetAssetRecord(Uuid id)
		{
			return m_AssetRegistry.GetRecord(id);
		}

		static AssetType GetAssetType(Uuid id)
		{
			auto metadata = m_AssetRegistry.GetRecord(id);
			if (!metadata)
			{
				return AssetType::Unknown;
			}
			return metadata->type;
		}

		static Asset* CreateAsset(const std::filesystem::path& filepath)
		{
			auto metadata = m_AssetRegistry.Create(filepath);
			if (!metadata)
			{
				return nullptr;
			}

			if (IsAssetLoaded(metadata->id))
			{
				return m_LoadedAssets[metadata->id];
			}

			Asset* asset = AssetImporter::ImportAsset(*metadata);
			if (!asset)
			{
				LOG_ERROR("AssetManager::ImportAsset - asset import failed! '{0}'", filepath.string());
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