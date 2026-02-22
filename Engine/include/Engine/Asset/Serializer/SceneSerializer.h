#pragma once

namespace Engine
{
	class Scene;

	class SceneSerializer
	{
	public:
		static void Serialize(Scene* scene, const std::filesystem::path& path);
		static Scene* Deserialize(const std::filesystem::path& path);
	};
}