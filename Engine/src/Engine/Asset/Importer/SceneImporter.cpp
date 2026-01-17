#include "enginepch.h"
#include "SceneImporter.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	Scene* SceneImporter::ImportScene(const AssetRecord& metadata)
	{
		return LoadScene(Project::GetAssetsDirectory() / metadata.path);
	}

	Scene* SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		Scene* scene = SceneSerializer::Deserialize(path);
		return scene;
	}
}