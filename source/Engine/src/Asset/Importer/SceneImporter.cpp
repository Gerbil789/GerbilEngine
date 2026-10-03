#include "enginepch.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Serializer/Serializer.h"

namespace engine
{
	SceneAsset SceneImporter::LoadScene(const std::filesystem::path& path)
  {
    return Serializer::DeserializeScene(path);
  }
}