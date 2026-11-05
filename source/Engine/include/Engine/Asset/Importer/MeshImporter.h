#pragma once

#include "Engine/Graphics/Mesh.h"

namespace engine
{
	class MeshImporter
	{
	public:
		static Mesh LoadMesh(const std::filesystem::path& path);
	};
}