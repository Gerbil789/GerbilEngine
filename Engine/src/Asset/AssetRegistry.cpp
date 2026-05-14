#include "enginepch.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"
#include <glaze/glaze.hpp>
#include <fstream>

//TODO: also keep track of directories

template <>
struct glz::meta<Engine::AssetType> {
	using enum Engine::AssetType;
	static constexpr auto value = enumerate(
		"Unknown", Unknown,
		"Texture", Texture2D,
		"Mesh", Mesh,
		"Shader", Shader,
		"Material", Material,
		"Audio", Audio,
		"Scene", Scene,
		"Script", Script,
		"Other", Other,
		"Directory", Directory,
		"EmptyDirectory", EmptyDirectory
	);
};

// Assuming Uuid contains a uint64_t. We tell Glaze to treat Uuid as its underlying integer.
template <>
struct glz::meta<Engine::Uuid> {
	static constexpr auto value = [](auto& self) -> auto& {
		// Cast to uint64_t reference. Adjust this if your Uuid internal member is named differently!
		return reinterpret_cast<uint64_t&>(self);
		};
};

template <>
struct glz::meta<Engine::AssetRecord> {
	using T = Engine::AssetRecord;
	static constexpr auto value = object(
		"ID", &T::id,
		"Path", &T::path, // Glaze supports std::filesystem::path natively!
		"Type", &T::type
	);
};

namespace Engine
{
	// A tiny wrapper just for the root JSON object
	struct RegistryFile 
	{
		std::vector<AssetRecord> Assets;
	};

	void AssetRegistry::Load(const std::filesystem::path& path) //TODO: split into functions (load from file, scan directory, ...) and make better logs
	{
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Asset registry file '{}' does not exist.", path);
			return;
		}


		// 1. Parse directly into our wrapper struct
		RegistryFile fileData;
		std::string buffer; // <--- ADD THIS: The memory buffer Glaze will use

		// Pass the buffer as the 3rd argument
		auto ec = glz::read_file_json(fileData, path.string(), buffer);

		if (ec) {
			LOG_ERROR("Registry Load Error: {}", glz::format_error(ec, buffer)); // Bonus: passing the buffer here gives you the exact line/character of the error!
			return;
		}
		auto assetsDir = Engine::Project::GetActive().GetAssetsDirectory();
		m_Records.clear();

		// 2. Process the loaded assets
		for (auto& record : fileData.Assets)
		{
			record.path = assetsDir / record.path;

			if (record.type == AssetType::Other || !std::filesystem::exists(record.path))
				continue;

			m_Records[record.id] = std::move(record);
		}

		// 3. Scan disk for new assets
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
		auto assetsDir = Engine::Project::GetActive().GetAssetsDirectory();

		RegistryFile outData;
		outData.Assets.reserve(m_Records.size());

		for (const auto& [id, record] : m_Records)
		{
			AssetRecord diskCopy = record;
			// Convert to relative path for storage
			diskCopy.path = std::filesystem::relative(record.path, assetsDir);
			outData.Assets.push_back(std::move(diskCopy));
		}

		std::string buffer;
		// Write the wrapper struct to disk
		auto ec = glz::write_file_json(outData, path.string(), buffer);

		if (ec) {
			LOG_ERROR("Failed to save registry: {}", glz::format_error(ec));
		}
	}


	const AssetRecord* AssetRegistry::Create(const std::filesystem::path& path)
	{
		auto assetsDir = Engine::Project::GetActive().GetAssetsDirectory();

		for (const auto& [uuid, record] : m_Records)
		{
			if (record.path == path)
			{
				LOG_WARNING("Asset '{}' already exists in registry.", path);
				return &record;
			}
		}

		auto id = Uuid(); // generate new UUID
		auto [it, inserted] = m_Records.try_emplace(id, AssetRecord{id, assetsDir / path, GetAssetTypeFromExtension(path.extension().string())});

		Save(Engine::Project::GetActive().GetProjectDirectory() / "assetRegistry.json");
		LOG_TRACE("Added asset '{}' to registry.", path);
		return &it->second;
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
			auto assetsDir = Engine::Project::GetActive().GetAssetsDirectory();
			return std::filesystem::relative(it->second.path, assetsDir);
		}
		return emptyPath;

	}

	std::vector<const AssetRecord*> AssetRegistry::GetAllRecords() const
	{
		std::vector<const AssetRecord*> records;
		records.reserve(m_Records.size());
		for (const auto& [id, record] : m_Records)
		{
			records.push_back(&record);
		}
		return records;
	}

	void AssetRegistry::Clear()
	{ 
		m_Records.clear(); 
	}
}