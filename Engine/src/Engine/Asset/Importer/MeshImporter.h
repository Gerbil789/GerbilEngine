#pragma once

#include "Engine/Asset/AssetRecord.h"
#include "Engine/Graphics/Mesh.h"

namespace Engine
{
	class MeshImporter
	{
	public:
		static Mesh* ImportMesh(const AssetRecord& metadata);
		static Mesh* LoadMesh(const std::filesystem::path& path);
	};
}