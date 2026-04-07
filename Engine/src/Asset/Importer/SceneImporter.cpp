#include "enginepch.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

namespace Engine
{
	Scene* SceneImporter::ImportScene(const std::filesystem::path& path)
	{
		return LoadScene(path);
	}

	Scene* SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		Scene* scene = SceneSerializer::Deserialize(path);
		return scene;
	}
}