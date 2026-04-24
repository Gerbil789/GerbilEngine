#pragma once

#include "Engine/Core/API.h"
#include <optional>

namespace Engine
{
	class Mesh;

	class ENGINE_API MeshImporter
	{
	public:
		static std::optional<Mesh> LoadMesh(const std::filesystem::path& path);
	};
}