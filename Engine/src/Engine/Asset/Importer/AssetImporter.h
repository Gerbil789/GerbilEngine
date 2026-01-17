#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRecord.h"

namespace Engine
{
	class AssetImporter
	{
	public:
		static Asset* ImportAsset(const AssetRecord& metadata);
	};
}