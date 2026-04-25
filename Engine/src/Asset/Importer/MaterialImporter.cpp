#include "enginepch.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"

namespace Engine
{
	std::optional<Material> MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		return MaterialSerializer::Deserialize(path);
	}
}