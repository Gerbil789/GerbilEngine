#pragma once

#include <Engine/Asset/AssetRecord.h>

namespace Engine
{
	class AssetRegistry
	{
	public:
		void Load(const std::filesystem::path& path);
		void Save(const std::filesystem::path& path);

		bool IsUUIDValid(const UUID& id) const { return id.IsValid() && m_Records.find(id) != m_Records.end(); }
		const AssetRecord* Create(const std::filesystem::path& path);

		const UUID GetUUIDFromPath(const std::filesystem::path& path) const;
		const AssetRecord* GetRecord(const UUID& id) const;

		std::filesystem::path GetPath(const UUID& id) const;
		std::filesystem::path GetRelativePath(const UUID& id) const;

		std::vector<const AssetRecord*> GetAllRecords() const
		{
			std::vector<const AssetRecord*> records;
			records.reserve(m_Records.size());
			for (const auto& [id, metadata] : m_Records)
			{
				records.push_back(&metadata);
			}

			return records;
		}

		void Clear() { m_Records.clear(); }

	private:
		std::unordered_map<UUID, AssetRecord> m_Records;
	};
}