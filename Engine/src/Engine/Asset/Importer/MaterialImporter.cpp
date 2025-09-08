#include "enginepch.h"
#include "MaterialImporter.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	Ref<Material> MaterialImporter::ImportMaterial(UUID id, const AssetMetadata& metadata)
	{
		return LoadMaterial(Project::GetAssetsDirectory() / metadata.path);
	}

	Ref<Material> MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		Ref<Material> material = MaterialSerializer::Deserialize(path);
		return material;
	}
}