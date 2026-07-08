#pragma once

#include <Engine/Asset/AssetRecord.h>
#include <unordered_map>

namespace Engine
{
	struct DirectoryNode
	{
		std::unordered_map<std::string, DirectoryNode> subdirectories;
		std::vector<Uuid> assets;
	};

	class ENGINE_API AssetRegistry
	{
	public:
		void Load();
		void Save();

		bool Exists(Uuid id) const;
		void AddRecord(Uuid id, const std::filesystem::path& path);
		void RemoveRecord(Uuid id);

		const AssetRecord& GetRecord(Uuid id) const;
		Uuid GetIdFromPath(const std::filesystem::path& path) const;
		const DirectoryNode* GetDirectoryNode(const std::filesystem::path& relativePath) const;
		AssetType GetType(Uuid id) const;

		std::filesystem::path GetPath(const Uuid& id) const;
		std::filesystem::path GetRelativePath(const Uuid& id) const;

	private:
		void ScanDirectory(const std::filesystem::path& directory);
		void RebuildVirtualFileSystem();
		void AddToVFS(const AssetRecord& record);

	private:
		std::unordered_map<Uuid, AssetRecord> m_Records;
		DirectoryNode m_RootNode;
	};
}