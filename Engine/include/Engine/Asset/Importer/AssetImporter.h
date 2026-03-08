#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Asset;
	struct AssetRecord;

	class ENGINE_API AssetImporter
	{
	public:
		static Asset* ImportAsset(const AssetRecord& metadata);
	};
}