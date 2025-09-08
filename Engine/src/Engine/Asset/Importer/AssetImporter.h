#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetMetadata.h"

namespace Engine
{
	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(UUID id, const AssetMetadata& metadata); //TODO: do i need ID as separate param?
	};
}