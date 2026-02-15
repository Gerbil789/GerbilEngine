#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRecord.h"

namespace Engine
{
	class ENGINE_API AssetSerializer
	{
	public:
		static void SerializeAsset(Asset* asset, const AssetRecord& metadata);
		static Asset* DeserializeAsset(const AssetRecord& metadata);
	};
}