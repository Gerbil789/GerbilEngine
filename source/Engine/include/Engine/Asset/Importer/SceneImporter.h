#pragma once

namespace engine
{
	class SceneAsset;

	class SceneImporter
	{
	public:
		static std::optional<SceneAsset> LoadScene(const std::filesystem::path& path);
	};
}