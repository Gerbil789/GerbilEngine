#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Renderer/Mesh.h"

namespace Engine
{
	class MeshImporter
	{
	public:
		static Ref<Mesh> ImportMesh(const AssetMetadata& metadata);
		static Ref<Mesh> LoadMesh(const std::filesystem::path& path);
	};
}