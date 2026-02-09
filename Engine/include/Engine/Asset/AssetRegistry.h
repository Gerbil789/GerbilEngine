#pragma once

#include "Engine/Core/API.h"
#include <Engine/Asset/AssetRecord.h>

namespace Engine
{
	class ENGINE_API AssetRegistry
	{
	public:
		void Load(const std::filesystem::path& path);
		void Save(const std::filesystem::path& path);

		bool IsUUIDValid(const Uuid& id) const;
		const AssetRecord* Create(const std::filesystem::path& path);

		const Uuid GetUUIDFromPath(const std::filesystem::path& path) const;
		const AssetRecord* GetRecord(const Uuid& id) const;

		std::filesystem::path GetPath(const Uuid& id) const;
		std::filesystem::path GetRelativePath(const Uuid& id) const;

		std::vector<const AssetRecord*> GetAllRecords() const;
		void Clear();
	};
}