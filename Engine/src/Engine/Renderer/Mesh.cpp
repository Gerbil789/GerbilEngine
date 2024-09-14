#include "enginepch.h"
#include "Engine/Renderer/Mesh.h"

#include <fbxsdk.h>

namespace Engine
{
	Ref<Asset> MeshFactory::Load(const std::string& filePath)
	{
		//TODO: implement this  
		Ref<Mesh> mesh = CreateRef<Mesh>(filePath);
        return mesh;
	}

	Ref<Asset> MeshFactory::Create(const std::string& filePath)
	{
		// meshes should not be created in engine, they should be loaded from file
		ENGINE_LOG_ERROR("MeshFactory::Create not implemented");
		return nullptr;
	}
}