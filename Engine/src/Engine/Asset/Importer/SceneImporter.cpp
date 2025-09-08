#include "enginepch.h"
#include "SceneImporter.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	Ref<Scene> SceneImporter::ImportScene(UUID id, const AssetMetadata& metadata)
	{
		return LoadScene(Project::GetAssetsDirectory() / metadata.path);
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		auto scene = SceneSerializer::Deserialize(path);
		return scene;
	}
}