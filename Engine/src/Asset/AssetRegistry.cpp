#include "enginepch.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Engine.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Engine
{
	std::unordered_map<Uuid, AssetRecord> m_Records;

	void AssetRegistry::Load(const std::filesystem::path& path) //TODO: split into functions (load from file, scan directory, ...) and make better logs
	{
		if (!std::filesystem::exists(path))
		{
			LOG_WARNING("Asset registry file '{}' does not exist.", path);
			return;
		}

		YAML::Node data = YAML::LoadFile(path.string());
		if (!data["Assets"])
		{
			return;
		}

		auto assetsDir = Engine::GetAssetsDirectory();
		m_Records.clear();
		for (const auto& entry : data["Assets"])
		{
			AssetRecord record;
			record.id = entry["ID"].as<uint64_t>();
			std::filesystem::path relativePath = entry["Path"].as<std::string>();
			record.path = assetsDir / relativePath;
			record.type = AssetTypeFromString(entry["Type"].as<std::string>());

			//auto type = GetAssetTypeFromExtension(record.path.extension().string());
			//if (type == AssetType::Unknown)
			//{
			//	LOG_ERROR("Asset '{}' has unknown type, skipping.", record.path);
			//	continue;
			//}

			if (record.type == AssetType::Other)
			{
				continue; // Skip 'Other' types like .txt, .md, etc.
			}

			if (!std::filesystem::exists(record.path))
			{
				LOG_WARNING("Asset '{}' not found on disk, skipping.", record.path);
				continue;
			}

			m_Records[record.id] = std::move(record);
		}

		for (auto& file : std::filesystem::recursive_directory_iterator(assetsDir))
		{
			if (!file.is_regular_file() || file.is_directory())
			{
				continue;	// Skip non-regular files (directories, symlinks, etc.)
			}

			auto ext = file.path().extension().string();
			if (ext.empty())
			{
				continue; // Skip files without extension
			}

			auto type = GetAssetTypeFromExtension(ext);
			if (type == AssetType::Unknown)
			{
				LOG_WARNING("File '{}' has unknown asset type, skipping.", file.path());
				continue;
			}

			if (type == AssetType::Other)
			{
				continue; // Skip 'Other' types like .txt, .md, etc.
			}

			bool found = false;
			for (auto& [id, meta] : m_Records)
			{
				if (meta.path == file.path())
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				AssetRecord record;
				record.id = Uuid(); // generate new UUID
				record.path = file.path();
				record.type = type;
				LOG_INFO("Discovered new asset '{}'", record.path);
				m_Records[record.id] = std::move(record);
			}
		}

		Save(path);
	}

	void AssetRegistry::Save(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;

		auto assetsDir = Engine::GetAssetsDirectory();

		for (const auto& [uuid, record] : m_Records)
		{
			std::filesystem::path relativePath = std::filesystem::relative(record.path, assetsDir);

			if (relativePath.empty())
			{
				LOG_WARNING("Asset '{}' is outside of the assets directory, skipping.", record.path);
				continue;
			}

			out << YAML::BeginMap;
			out << YAML::Key << "ID" << YAML::Value << (uint64_t)record.id;
			out << YAML::Key << "Path" << YAML::Value << relativePath.string();
			out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(record.type);
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool AssetRegistry::IsUUIDValid(const Uuid& id) const
	{
		return id.IsValid() && m_Records.find(id) != m_Records.end();
	}

	const AssetRecord* AssetRegistry::Create(const std::filesystem::path& path)
	{
		if (auto id = GetUUIDFromPath(path); id.IsValid())
		{
			LOG_WARNING("Asset '{}' already exists in registry.", path);
			return GetRecord(id);
		}

		Uuid id = Uuid();

		auto [it, inserted] = m_Records.try_emplace(
			id,
			AssetRecord{
					id,
					path,
					GetAssetTypeFromExtension(path.extension().string())
			}
		);

		Save(Engine::GetProjectDirectory() / "assetRegistry.yaml");
		LOG_TRACE("Added asset '{}' to registry.", path);
		return &it->second;
	}

	const Uuid AssetRegistry::GetUUIDFromPath(const std::filesystem::path& path) const
	{
		for (const auto& [uuid, record] : m_Records)
		{
			if (record.path == path)
			{
				return record.id;
			}
		}
		static Uuid id(0);
		return id;
	}

	const AssetRecord* AssetRegistry::GetRecord(const Uuid& id) const
	{
		if (auto it = m_Records.find(id); it != m_Records.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	std::filesystem::path AssetRegistry::GetPath(const Uuid& id) const
	{
		static std::filesystem::path emptyPath;
		if (auto it = m_Records.find(id); it != m_Records.end())
		{
			return it->second.path;
		}

		LOG_ERROR("AssetRegistry::GetPath - UUID '{}' not found in registry.", id);
		return emptyPath;
	}

	std::filesystem::path AssetRegistry::GetRelativePath(const Uuid& id) const
	{
		static std::filesystem::path emptyPath;
		if (auto it = m_Records.find(id); it != m_Records.end())
		{
			auto assetsDir = Engine::GetAssetsDirectory();
			return std::filesystem::relative(it->second.path, assetsDir);
		}
		return emptyPath;

	}

	std::vector<const AssetRecord*> AssetRegistry::GetAllRecords() const
	{
		std::vector<const AssetRecord*> records;
		records.reserve(m_Records.size());
		for (const auto& [id, metadata] : m_Records)
		{
			records.push_back(&metadata);
		}

		return records;
	}

	void AssetRegistry::Clear()
	{ 
		m_Records.clear(); 
	}


}