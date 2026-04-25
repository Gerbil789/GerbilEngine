#include "enginepch.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

namespace Engine
{
	std::optional<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
  {
    return  SceneSerializer::Deserialize(path);
  }
}