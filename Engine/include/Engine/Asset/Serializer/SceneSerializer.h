#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Scene;
	class ScriptRegistry;

	class ENGINE_API SceneSerializer
	{
	public:
		static void Initialize(ScriptRegistry& registry);

		static void Serialize(Scene* scene, const std::filesystem::path& path);
		static Scene* Deserialize(const std::filesystem::path& path);
	};
}