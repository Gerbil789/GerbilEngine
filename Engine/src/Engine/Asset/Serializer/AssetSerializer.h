#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetMetadata.h"

namespace Engine
{
	class AssetSerializer
	{
	public:
		static void SerializeAsset(const Ref<Asset>& asset, const AssetMetadata& metadata);
		static Asset* DeserializeAsset(const AssetMetadata& metadata);
	};
}