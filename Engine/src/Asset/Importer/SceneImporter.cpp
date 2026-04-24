#include "enginepch.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

namespace Engine
{
	std::optional<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
  {
		Scene* scene = SceneSerializer::Deserialize(path); //TODO: change to return optional from deserialize and remove this function
    if (scene)
    {
      std::optional<Scene> result = std::move(*scene);
      delete scene;
      return result; // Return by value
    }
    return std::nullopt;
  }
}