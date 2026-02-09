#pragma once

#include "Engine/Asset/AssetRecord.h"
#include "Engine/Graphics/Material.h"

namespace Engine
{
	class MaterialImporter
	{
	public:
		static Material* ImportMaterial(const AssetRecord& record);
		static Material* LoadMaterial(const std::filesystem::path& path);
	};
}