#pragma once

namespace Engine
{
	class SceneAsset;

	class SceneImporter
	{
	public:
		static std::optional<SceneAsset> LoadScene(const std::filesystem::path& path);
	};
}