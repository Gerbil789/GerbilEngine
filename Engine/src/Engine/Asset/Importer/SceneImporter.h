#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneImporter
	{
	public:
		static Scene* ImportScene(const AssetMetadata& metadata);
		static Scene* LoadScene(const std::filesystem::path& path);
	};
}