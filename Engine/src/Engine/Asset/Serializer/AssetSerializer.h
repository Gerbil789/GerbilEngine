#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRecord.h"

namespace Engine
{
	class AssetSerializer
	{
	public:
		static void SerializeAsset(Asset* asset, const AssetRecord& metadata);
		static Asset* DeserializeAsset(const AssetRecord& metadata);
	};
}