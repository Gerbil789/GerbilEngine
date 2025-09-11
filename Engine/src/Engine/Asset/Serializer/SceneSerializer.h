#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneSerializer
	{
	public:
		static void Serialize(const Ref<Scene>& scene, const std::filesystem::path& path);
		static Ref<Scene> Deserialize(const std::filesystem::path& path);
	};
}