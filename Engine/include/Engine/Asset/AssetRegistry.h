#pragma once

#include <Engine/Asset/AssetRecord.h>
#include <unordered_map>
#include <unordered_set>

namespace Engine
{
	class ENGINE_API AssetRegistry
	{
	public:
		void Load(const std::filesystem::path& path);
		void Save(const std::filesystem::path& path);

		const void Create(Uuid id, const std::filesystem::path& path);

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


		template<typename Func>
		void ForEachRecord(Func&& callback) const
		{
			for (const auto& [id, record] : m_Records)
			{
				callback(record);
			}
		}

		template<typename Func>
		void ForEachRecord(AssetType type, Func&& callback) const
		{
			for (const auto& [id, record] : m_Records)
			{
				if (record.type == type)
				{
					callback(record);
				}
			}
		}

		template<typename Func>
		void ForEachDirty(Func&& func)
		{
			for (Uuid id : m_DirtySet)
			{
				auto it = m_Records.find(id);
				if (it == m_Records.end())
					continue;

				func(it->second);
			}
		}

		AssetType GetType(Uuid id) const
		{
			const AssetRecord& record = GetRecord(id);
			if (!record)
			{
				return AssetType::Unknown;
			}
			return record.type;
		}

		std::filesystem::path GetPath(const Uuid& id) const;
		std::filesystem::path GetRelativePath(const Uuid& id) const;

		void MarkDirty(Uuid id);
		void ClearDirtySet();

		void Clear();

	private:
		void ScanDirectory(const std::filesystem::path& directory);

	private:
		std::unordered_map<Uuid, AssetRecord> m_Records;
		std::unordered_set<Uuid> m_DirtySet;
	};
}