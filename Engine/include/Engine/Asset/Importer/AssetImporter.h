#pragma once

#include "Engine/Core/API.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRecord.h"

namespace Engine
{
	class ENGINE_API AssetImporter
	{
	public:
		static Asset* ImportAsset(const AssetRecord& metadata);
	};
}