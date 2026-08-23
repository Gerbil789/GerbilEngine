#pragma once

#include <optional>

namespace Engine
{
	class MeshAsset;

	class MeshImporter
	{
	public:
		static std::optional<MeshAsset> LoadMesh(const std::filesystem::path& path);
	};
}