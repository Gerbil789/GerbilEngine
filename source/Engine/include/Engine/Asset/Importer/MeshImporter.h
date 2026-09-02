#pragma once

#include <optional>

namespace engine
{
	class MeshAsset;

	class MeshImporter
	{
	public:
		static std::optional<MeshAsset> LoadMesh(const std::filesystem::path& path);
	};
}