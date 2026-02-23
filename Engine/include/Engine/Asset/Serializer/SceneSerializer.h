#pragma once

namespace Engine
{
	class Scene;
	class ScriptRegistry;

	class SceneSerializer
	{
	public:
		static void Serialize(Scene* scene, const std::filesystem::path& path, ScriptRegistry& registry);
		static Scene* Deserialize(const std::filesystem::path& path);
	};
}