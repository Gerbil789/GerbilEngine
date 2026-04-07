#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Mesh;

	class ENGINE_API MeshImporter
	{
	public:
		static Mesh* ImportMesh(const std::filesystem::path& path);
		static Mesh* LoadMesh(const std::filesystem::path& path);
	};
}