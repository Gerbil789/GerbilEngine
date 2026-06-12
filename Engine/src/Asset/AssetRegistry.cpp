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

	void AssetRegistry::Load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LOG_WARNING("Asset registry file '{}' does not exist, creating new one...", path);
			std::ofstream outFile(path);
			if (!outFile)
			{
				LOG_ERROR("Failed to create asset registry file at '{}'", path);
				return;
			}
			outFile << "{\n  \"Assets\": []\n}"; // Write an empty registry structure
		}

		RegistryFile fileData;
		std::string buffer;
		auto assetsDir = Engine::Project::GetActive().GetAssetsDirectory();
		m_Records.clear();

		auto ec = glz::read_file_json(fileData, path.string(), buffer);

		if (ec) 
		{
			LOG_ERROR("Registry Load Error: {}", glz::format_error(ec, buffer));
			return;
		}

		for (auto& record : fileData.Assets)
		{
			record.path = assetsDir / record.path;

			if (record.type == AssetType::Other || !std::filesystem::exists(record.path))
				continue;

			m_Records[record.id] = std::move(record);
		}

		ScanDirectory(assetsDir); // look for new files

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
			diskCopy.path = std::filesystem::relative(record.path, assetsDir);
			outData.Assets.push_back(std::move(diskCopy));
		}

		std::string buffer;
		auto ec = glz::write_file_json(outData, path.string(), buffer);

		if (ec) 
		{
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
				LOG_WARNING("Asset '{}' already exists", path);
				return &record;
			}
		}

		Uuid id = Uuid::Generate();
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

	void AssetRegistry::ScanDirectory(const std::filesystem::path& directory)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			if (!entry.is_regular_file()) continue;

			const auto& path = entry.path();
			if (!path.has_extension()) continue;

			AssetType type = GetAssetTypeFromExtension(path.extension().string());

			if (type == AssetType::Other) continue;
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
}