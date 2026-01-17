#pragma once

#include "Engine/Asset/AssetRecord.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneImporter
	{
	public:
		static Scene* ImportScene(const AssetRecord& metadata);
		static Scene* LoadScene(const std::filesystem::path& path);
	};
}