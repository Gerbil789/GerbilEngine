#pragma once

namespace Engine
{
	class Scene;

	class SceneImporter
	{
	public:
		static Scene* ImportScene(const std::filesystem::path& path);
		static Scene* LoadScene(const std::filesystem::path& path);
	};
}