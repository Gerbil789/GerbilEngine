#pragma once

#include <optional>

namespace Engine
{
	class MeshAsset;

	class ENGINE_API MeshImporter
	{
	public:
		static std::optional<MeshAsset> LoadMesh(const std::filesystem::path& path);
	};
}