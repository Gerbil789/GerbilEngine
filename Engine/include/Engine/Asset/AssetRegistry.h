#pragma once

#include <Engine/Asset/AssetRecord.h>

namespace Engine
{
	class ENGINE_API AssetRegistry
	{
	public:
		void Load(const std::filesystem::path& path);
		void Save(const std::filesystem::path& path);

		const AssetRecord* Create(const std::filesystem::path& path);

		template<typename Self>
		AssetRecord& GetRecord(this Self&& self, Uuid id)
		{
			if (auto it = ((AssetRegistry&)self).m_Records.find(id); it != ((AssetRegistry&)self).m_Records.end())
			{
				return it->second;
			}

			static AssetRecord invalidRecord;
			return invalidRecord;
		}

		template<typename Self>
		std::vector<AssetRecord*> GetRecords(this Self&& self, AssetType type)
		{
			std::vector<AssetRecord*> records;
			for (auto& [id, record] : ((AssetRegistry&)self).m_Records)
			{
				if (record.type == type)
				{
					records.push_back(&record);
				}
			}
			return records;
		}

		AssetType GetType(Uuid id) const
		{
			const AssetRecord& record = GetRecord(id);
			if (!record.IsValid())
			{
				return AssetType::Unknown;
			}
			return record.type;
		}

		std::filesystem::path GetPath(const Uuid& id) const;
		std::filesystem::path GetRelativePath(const Uuid& id) const;
		std::vector<const AssetRecord*> GetAllRecords() const;
		void Clear();

	private:
		std::unordered_map<Uuid, AssetRecord> m_Records;
	};
}