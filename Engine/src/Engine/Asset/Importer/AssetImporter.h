#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetMetadata.h"

namespace Engine
{
	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(const AssetMetadata& metadata);
	};
}