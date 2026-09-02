#include "enginepch.h"
#include "Engine/Asset/AssetRegistry.h"
// #include "Engine/Utility/File.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include <glaze/glaze.hpp>
#include <fstream>

template <>
struct glz::meta<engine::Uuid> {
	static constexpr auto value = [](auto& self) -> auto& {
		return reinterpret_cast<uint64_t&>(self);
		};
};

template <>
struct glz::meta<engine::AssetRecord> {
	static constexpr auto value = object(
		"ID", &engine::AssetRecord::id,
		"Path", &engine::AssetRecord::path
	);
};

namespace engine
{
	void AssetRegistry::Load()
	{
		std::filesystem::path path = Project::Directory() / "assetRegistry.json";

    LOG_TRACE("Project Directory: {}", Project::Directory());

		if (!std::filesystem::exists(path))
		{
			LOG_WARNING("Asset registry file '{}' does not exist, creating new one...", path);
			std::ofstream outFile(path);
			if (!outFile)
			{
				throw std::runtime_error("Could not create asset registry file.");
			}
		}
		m_Records.clear();

		std::vector<AssetRecord> assets;
		std::string buffer;

		if (auto ec = glz::read_file_json(assets, path.string(), buffer))
		{
			LOG_ERROR("Registry Load Error: {}", glz::format_error(ec, buffer));
			return;
		}

		for (AssetRecord& record : assets)
		{
			record.path = Project::AssetsDirectory() / record.path;

			if (!std::filesystem::exists(record.path)) 
			{
				LOG_WARNING("Asset file '{}' does not exist, skipping...", record.path);
				continue;
			}
				
			record.type = GetAssetTypeFromExtension(record.path.extension().string());

			m_Records[record.id] = std::move(record);
		}

		ScanDirectory(Project::AssetsDirectory()); // look for new files
		RebuildVirtualFileSystem();
		Save();
	}

	void AssetRegistry::Save()
	{
		const std::filesystem::path& path = Project::Directory() / "assetRegistry.json";

		std::vector<AssetRecord> assets;
		assets.reserve(m_Records.size());

		for (const auto& [id, record] : m_Records)
		{
			AssetRecord diskCopy = record;
			diskCopy.path = std::filesystem::relative(record.path, Project::AssetsDirectory().generic_string());
			assets.push_back(std::move(diskCopy));
		}

		std::string buffer;
		auto ec = glz::write_file_json<glz::opts{ .prettify = true }>(assets, path.string(), buffer);


		if (ec) 
		{
			LOG_ERROR("Failed to save registry: {}", glz::format_error(ec));
		}
	}


	void AssetRegistry::AddRecord(Uuid id, const std::filesystem::path& path)
	{
		for (const auto& [uuid, record] : m_Records)
		{
			if (record.path == path)
			{
				LOG_WARNING("Asset '{}' already exists", path);
				return;
			}
		}

		auto type = GetAssetTypeFromExtension(path.extension().string());
		auto [it, inserted] = m_Records.try_emplace(id, AssetRecord{id, Project::AssetsDirectory() / path, type });

		if (inserted)
		{
			AddToVFS(m_Records[id]);
		}

		Save();
		LOG_TRACE("Added asset '{}' to registry.", path);
		return;
	}

	bool AssetRegistry::Exists(Uuid id) const
	{
		return m_Records.find(id) != m_Records.end();
	}

	void AssetRegistry::RemoveRecord(Uuid id)
	{
		const auto it = m_Records.find(id);

		if (it != m_Records.end())
		{
			RebuildVirtualFileSystem();
			m_Records.erase(it);
			LOG_TRACE("Removed asset '{}' from registry.", id);
			Save();
		}
		else
		{
			LOG_WARNING("Attempted to remove non-existent asset '{}' from registry.", id);
		}
	}

	static AssetRecord s_NullRecord{};

	const AssetRecord& AssetRegistry::GetRecord(Uuid id) const
	{
		if (auto it = m_Records.find(id); it != m_Records.end())
		{
			return it->second;
		}

		return s_NullRecord;
	}

	AssetType AssetRegistry::GetType(Uuid id) const
	{
		if (!Exists(id)) return AssetType::Unknown;
		const AssetRecord& record = GetRecord(id);
		return record.type;
	}

	std::filesystem::path AssetRegistry::GetPath(const Uuid& id) const
	{
		static std::filesystem::path emptyPath;
		if (auto it = m_Records.find(id); it != m_Records.end())
		{
			return it->second.path;
		}

		LOG_ERROR("UUID '{}' not found in registry.", id);
		return emptyPath;
	}

	std::filesystem::path AssetRegistry::GetRelativePath(const Uuid& id) const
	{
		static std::filesystem::path emptyPath;
		if (auto it = m_Records.find(id); it != m_Records.end())
		{
			return std::filesystem::relative(it->second.path, Project::AssetsDirectory());
		}
		return emptyPath; //TODO: return {}? and test it!!!

	}

	void AssetRegistry::ScanDirectory(const std::filesystem::path& directory)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			const auto& path = entry.path();

			if (!entry.is_regular_file() || !path.has_extension()) continue;

			AssetType type = GetAssetTypeFromExtension(path.extension().string());

			if (type == AssetType::Unknown)
			{
				LOG_WARNING("File '{}' has unknown asset type, skipping.", path);
				continue;
			}

			auto it = std::find_if(m_Records.begin(), m_Records.end(), [&path](const auto& pair) { return pair.second.path == path; });
			if (it == m_Records.end())
			{
				AssetRecord record{ Uuid::Generate(), path, type };

				LOG_INFO("Discovered new asset '{}'", path);
				m_Records[record.id] = std::move(record);
			}
		}
	}

	void AssetRegistry::RebuildVirtualFileSystem()
	{
		m_RootNode.subdirectories.clear();
		m_RootNode.assets.clear();

		for (const auto& [id, record] : m_Records)
		{
			AddToVFS(record);
		}
	}

	void AssetRegistry::AddToVFS(const AssetRecord& record)
	{
		// Get the path relative to the assets folder (e.g., "Textures/Props/box.png")
		std::filesystem::path relativePath = std::filesystem::relative(record.path, Project::AssetsDirectory());
		std::filesystem::path parentDir = relativePath.parent_path();

		DirectoryNode* currentNode = &m_RootNode;

		// Traverse down the tree, creating nodes if they don't exist
		if (!parentDir.empty() && parentDir != ".")
		{
			for (const auto& component : parentDir)
			{
				currentNode = &currentNode->subdirectories[component.string()];
			}
		}

		// Add the asset ID to the final directory node
		currentNode->assets.push_back(record.id);
	}

	const DirectoryNode* AssetRegistry::GetDirectoryNode(const std::filesystem::path& relativePath) const
	{
		const DirectoryNode* currentNode = &m_RootNode;

		if (!relativePath.empty() && relativePath != ".")
		{
			for (const auto& component : relativePath)
			{
				auto it = currentNode->subdirectories.find(component.string());
				if (it == currentNode->subdirectories.end())
				{
					return nullptr; // Directory doesn't exist in VFS
				}
				currentNode = &it->second;
			}
		}

		return currentNode;
	}

	Uuid AssetRegistry::GetIdFromPath(const std::filesystem::path& path) const
	{
		const DirectoryNode* current = &m_RootNode;

		for (const auto& part : path)
		{
			// Skip root slash or empty parts
			if (part == "/" || part.empty()) continue;

			auto it = current->subdirectories.find(part.string());
			if (it == current->subdirectories.end())
			{
				return Uuid{};
			}
			current = &it->second;
		}

		return current->assets.empty() ? Uuid{} : current->assets.front();
	}
}