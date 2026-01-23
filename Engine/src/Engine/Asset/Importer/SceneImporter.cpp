#include "enginepch.h"
#include "SceneImporter.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Core/Engine.h"

namespace Engine
{
	Scene* SceneImporter::ImportScene(const AssetRecord& record)
	{
		return LoadScene(Engine::GetAssetsDirectory() / record.path);
	}

	Scene* SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		Scene* scene = SceneSerializer::Deserialize(path);
		return scene;
	}
}