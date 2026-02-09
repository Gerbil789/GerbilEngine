#include "enginepch.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Core/Engine.h"

namespace Engine
{
	Material* MaterialImporter::ImportMaterial(const AssetRecord& record)
	{
		return LoadMaterial(Engine::GetAssetsDirectory() / record.path);
	}

	Material* MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		Material* material = MaterialSerializer::Deserialize(path);
		return material;
	}
}