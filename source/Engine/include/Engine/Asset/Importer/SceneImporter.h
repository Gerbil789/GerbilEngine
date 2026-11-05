#pragma once

#include "Engine/Core/Scene.h"

namespace engine
{
	class SceneImporter
	{
	public:
		static Scene LoadScene(const std::filesystem::path& path);
	};
}