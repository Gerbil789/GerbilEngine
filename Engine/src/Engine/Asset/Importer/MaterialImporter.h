#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Graphics/Material.h"

namespace Engine
{
	class MaterialImporter
	{
	public:
		static Material* ImportMaterial(const AssetMetadata& metadata);
		static Material* LoadMaterial(const std::filesystem::path& path);
	};
}