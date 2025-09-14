#include "enginepch.h"
#include "AssetRegistry.h"
#include <yaml-cpp/yaml.h>
#include "Engine/Core/Project.h"

namespace Engine
{
	void AssetRegistry::Load(const std::filesystem::path& path) //TODO: split into functions (load from file, scan directory, ...) and make better logs
	{
		if (!std::filesystem::exists(path))
		{
			LOG_WARNING("Asset registry file '{}' does not exist.", path);
			return;
		}

		YAML::Node data = YAML::LoadFile(path.string());
		if (!data["Assets"])
			return;

		auto assetsDir = Project::GetAssetsDirectory();
		m_Records.clear();
		for (const auto& entry : data["Assets"])
		{
			AssetMetadata metadata;
			metadata.id = entry["ID"].as<uint64_t>();
			std::filesystem::path relativePath = entry["Path"].as<std::string>();
			metadata.path = assetsDir / relativePath;

			auto type = GetAssetTypeFromExtension(metadata.path.extension().string());
			if (type == AssetType::Unknown)
			{
				LOG_ERROR("Asset '{}' has unknown type, skipping.", metadata.path);
				continue;
			}

			if (type == AssetType::Other)
			{
				continue; // Skip 'Other' types like .txt, .md, etc.
			}

			if (!std::filesystem::exists(metadata.path))
			{
				LOG_WARNING("Asset '{}' not found on disk, skipping.", metadata.path);
				continue;
			}

			m_Records[metadata.id] = std::move(metadata);
		}

		for (auto& file : std::filesystem::recursive_directory_iterator(assetsDir))
		{
			if (!file.is_regular_file() || file.is_directory())
			{
				continue;	// Skip non-regular files (directories, symlinks, etc.)
			}

			auto ext = file.path().extension().string();
			if(ext.empty())
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
				AssetMetadata metadata;
				metadata.id = UUID(); // generate new UUID
				metadata.path = file.path();
				LOG_INFO("Discovered new asset '{}'", metadata.path);
				m_Records[metadata.id] = std::move(metadata);
			}
		}

		Save(path);
	}

	void AssetRegistry::Save(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;

		auto assetsDir = Engine::Project::GetAssetsDirectory();

		for (const auto& [uuid, metadata] : m_Records)
		{
			std::filesystem::path relativePath = std::filesystem::relative(metadata.path, assetsDir);

			if(relativePath.empty())
			{
				LOG_WARNING("Asset '{}' is outside of the assets directory, skipping.", metadata.path);
				continue;
			}

			out << YAML::BeginMap;
			out << YAML::Key << "ID" << YAML::Value << (uint64_t)metadata.id;
			out << YAML::Key << "Path" << YAML::Value << relativePath.string();
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	const AssetMetadata* AssetRegistry::Add(const std::filesystem::path& path)
	{
		auto id = GetUUIDFromPath(path);
		if (id.IsValid())
		{
			LOG_WARNING("Asset '{}' already exists in registry.", path);
			return GetMetadata(id);
		}

		AssetMetadata record;
		id = UUID();
		record.id = id;
		record.path = path;
		m_Records[id] = std::move(record);
		Save(Project::GetProjectDirectory() / "assetRegistry.yaml");
		LOG_TRACE("Added asset '{}' to registry.", record.path);
		return &m_Records[id];
	}

	const UUID& AssetRegistry::GetUUIDFromPath(const std::filesystem::path& path) const
	{
		for (const auto& [uuid, record] : m_Records)
		{
			if (record.path == path)
			{
				return record.id;
			}
		}
		static UUID id(0);
		return id;
	}

	const AssetMetadata* AssetRegistry::GetMetadata(const UUID& id) const
	{
		if (auto it = m_Records.find(id); it != m_Records.end())
		{
			return &it->second;
		}
		return nullptr;
	}
}