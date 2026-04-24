#include "enginepch.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"

namespace Engine
{
	std::optional<Material> MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		Material* material = MaterialSerializer::Deserialize(path); //TODO: change to return optional from deserialize and remove this function
		if (material)
			return *material;
		return std::nullopt;
	}
}