#include "enginepch.h"
#include "MaterialImporter.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	Material* MaterialImporter::ImportMaterial(const AssetMetadata& metadata)
	{
		return LoadMaterial(Project::GetAssetsDirectory() / metadata.path);
	}

	Material* MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		Material* material = MaterialSerializer::Deserialize(path);
		return material;
	}
}