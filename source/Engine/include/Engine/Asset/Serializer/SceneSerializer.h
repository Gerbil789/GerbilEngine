#pragma once

#include "Engine/Core/Scene.h"
#include <optional>
#include <filesystem>

namespace engine
{
	class SceneSerializer
	{
	public:
		static void Serialize(Scene scene, const std::filesystem::path& path);
		static std::optional<SceneAsset> Deserialize(const std::filesystem::path& path);
	};
}