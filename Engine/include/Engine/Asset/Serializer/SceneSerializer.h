#pragma once

#include "Engine/Scene/Scene.h"
#include <optional>

namespace Engine
{
	class ENGINE_API SceneSerializer
	{
	public:
		static void Serialize(Scene& scene, const std::filesystem::path& path);
		static std::optional<Scene> Deserialize(const std::filesystem::path& path);
	};
}