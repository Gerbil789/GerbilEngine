#pragma once

#include "Engine/Graphics/Mesh.h"

namespace engine
{
	class MeshImporter
	{
	public:
		static MeshAsset LoadMesh(const std::filesystem::path& path);
	};
}