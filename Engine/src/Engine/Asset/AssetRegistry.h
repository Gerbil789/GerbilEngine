#pragma once

#include <Engine/Asset/AssetMetadata.h>
#include <filesystem>

namespace Engine
{
	class AssetRegistry
	{
	public:
		void Load(const std::filesystem::path& path);
		void Save(const std::filesystem::path& path);

		bool IsUUIDValid(const UUID& id) const { return id.IsValid() && m_Records.find(id) != m_Records.end(); }
		const AssetMetadata* Create(const std::filesystem::path& path);

		const UUID& GetUUIDFromPath(const std::filesystem::path& path) const;
		const AssetMetadata* GetMetadata(const UUID& id) const;

		std::filesystem::path GetPath(const UUID& id) const;
		std::filesystem::path GetRelativePath(const UUID& id) const;


		const std::unordered_map<UUID, AssetMetadata>& GetAll() const { return m_Records; }

		void Clear() { m_Records.clear(); }

	private:
		std::unordered_map<UUID, AssetMetadata> m_Records;
	};
}