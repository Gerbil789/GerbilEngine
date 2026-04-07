#include "enginepch.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"

namespace Engine
{
	Material* MaterialImporter::ImportMaterial(const std::filesystem::path& path)
	{
		return LoadMaterial(path);
	}

	Material* MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		Material* material = MaterialSerializer::Deserialize(path);
		return material;
	}
}