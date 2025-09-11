#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Renderer/Material.h"

namespace Engine
{
	class MaterialImporter
	{
	public:
		static Ref<Material> ImportMaterial(const AssetMetadata& metadata);
		static Ref<Material> LoadMaterial(const std::filesystem::path& path);
	};
}