#pragma once

#include "Engine/Asset/AssetRecord.h"
#include "Engine/Graphics/Mesh.h"

namespace Engine
{
	class ENGINE_API MeshImporter
	{
	public:
		static Mesh* ImportMesh(const AssetRecord& metadata);
		static Mesh* LoadMesh(const std::filesystem::path& path);
	};
}