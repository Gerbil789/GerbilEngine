#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(UUID id, const AssetMetadata& metadata);
		static Ref<Scene> LoadScene(const std::filesystem::path& path);
	};
}