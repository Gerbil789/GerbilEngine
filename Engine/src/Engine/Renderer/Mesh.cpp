#include "enginepch.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Core/Application.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine
{
	Ref<Asset> MeshFactory::Load(const std::filesystem::path& path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        //WindowOpenEvent windowOpenEvent("ImportMeshWindow", (void*)scene);
		//Application::Get().OnEvent(windowOpenEvent);


        if (!scene || !scene->mRootNode)
        {
            ENGINE_LOG_ERROR("Failed to load FBX file: " + path.string() + " (" + importer.GetErrorString() + ")");
            return nullptr;
        }

        // Assume we only want to process the first mesh
        aiMesh* mesh = scene->mMeshes[0];
        if (!mesh)
        {
            //ENGINE_LOG_ERROR("No meshes found in FBX file: " + path.string());
            return nullptr;
        }

        // Create a new Mesh object
        Ref<Mesh> newMesh = CreateRef<Mesh>(path);

        // Process vertices
        newMesh->m_VertexCount = mesh->mNumVertices;
        newMesh->m_Vertices.reserve(newMesh->m_VertexCount);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            aiVector3D pos = mesh->mVertices[i];
            newMesh->m_Vertices.emplace_back(pos.x, pos.y, pos.z);
        }

        // Process normals
        newMesh->m_Normals.reserve(mesh->mNumVertices);
        if (mesh->HasNormals())
        {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                aiVector3D normal = mesh->mNormals[i];
                newMesh->m_Normals.emplace_back(normal.x, normal.y, normal.z);
            }
        }

        // Process UVs
        newMesh->m_UVs.reserve(mesh->mNumVertices);
        if (mesh->HasTextureCoords(0))
        {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                aiVector3D uv = mesh->mTextureCoords[0][i];
                newMesh->m_UVs.emplace_back(uv.x, uv.y);
            }
        }

        // Process indices
        newMesh->m_Indices.reserve(mesh->mNumFaces * 3);
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            if (face.mNumIndices == 3)
            {
                newMesh->m_Indices.push_back(face.mIndices[0]);
                newMesh->m_Indices.push_back(face.mIndices[1]);
                newMesh->m_Indices.push_back(face.mIndices[2]);
            }
            else
            {
                ENGINE_LOG_WARNING("Face with more than 3 indices detected, which is not handled.");
            }
        }

        // Set polygon count
        newMesh->m_PolygonCount = mesh->mNumFaces;

        // Return the Mesh as an Asset
        return newMesh;
	}
}