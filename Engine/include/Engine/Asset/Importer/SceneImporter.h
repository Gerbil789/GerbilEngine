#pragma once

namespace Engine
{
	class Scene;

	class SceneImporter
	{
	public:
		static std::optional<Scene> LoadScene(const std::filesystem::path& path);
	};
}