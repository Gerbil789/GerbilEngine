#include "enginepch.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

namespace engine
{
	std::optional<SceneAsset> SceneImporter::LoadScene(const std::filesystem::path& path)
  {
    return SceneSerializer::Deserialize(path);
  }
}