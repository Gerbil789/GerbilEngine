#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Graphics/Mesh.h"

namespace Engine
{
	class MeshImporter
	{
	public:
		static Mesh* ImportMesh(const AssetMetadata& metadata);
		static Mesh* LoadMesh(const std::filesystem::path& path);
	};
}