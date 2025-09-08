#include "enginepch.h"
#include "AssetRegistry.h"
#include <yaml-cpp/yaml.h>
#include "Engine/Core/Project.h"

namespace Engine
{
	void AssetRegistry::Load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LOG_WARNING("Asset registry file '{}' does not exist.", path.string());
			return;
		}

		YAML::Node data = YAML::LoadFile(path.string());
		if (!data["Assets"])
			return;

		m_Records.clear();
		for (const auto& entry : data["Assets"])
		{
			AssetMetadata record;
			record.id = entry["ID"].as<uint64_t>();
			record.path = entry["Path"].as<std::string>();

			m_Records[record.id] = std::move(record);
		}
	}

	void AssetRegistry::Save(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;

		for (const auto& [uuid, record] : m_Records)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "ID" << YAML::Value << (uint64_t)record.id;
			out << YAML::Key << "Path" << YAML::Value << record.path.string();
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
		record.id = UUID();
		record.path = path;
		m_Records[record.id] = std::move(record);
		Save(Project::GetAssetsDirectory() / "AssetRegistry.yaml");
		LOG_TRACE("Added asset '{}' to registry.", path);
		return &m_Records[record.id];
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