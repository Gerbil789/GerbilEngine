#include "enginepch.h"
#include "Engine/Renderer/Mesh.h"

namespace Engine
{
	Ref<Asset> MeshFactory::Load(const std::string& filePath)
	{
        // Initialize FBX SDK
        FbxManager* sdkManager = FbxManager::Create();
        FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
        sdkManager->SetIOSettings(ios);

        FbxImporter* importer = FbxImporter::Create(sdkManager, "");
        if (!importer->Initialize(filePath.c_str(), -1, sdkManager->GetIOSettings()))
        {
			ENGINE_LOG_ERROR("Failed to initialize FBX importer: {0}", importer->GetStatus().GetErrorString());
            sdkManager->Destroy();
            return nullptr;
        }

        FbxScene* scene = FbxScene::Create(sdkManager, "Scene");
        importer->Import(scene);
        importer->Destroy();

        // Retrieve the mesh node
        FbxNode* rootNode = scene->GetRootNode();
        if (!rootNode)
        {
			ENGINE_LOG_ERROR("No root node found in FBX scene.");
            sdkManager->Destroy();
            return nullptr;
        }

        FbxMesh* fbxMesh = nullptr;
        for (int i = 0; i < rootNode->GetChildCount(); ++i)
        {
            FbxNode* node = rootNode->GetChild(i);
            if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                fbxMesh = (FbxMesh*)node->GetNodeAttribute();
                break;
            }
        }

        if (!fbxMesh)
        {
			ENGINE_LOG_ERROR("No mesh found in FBX scene.");
            sdkManager->Destroy();
            return nullptr;
        }

        Ref<Mesh> mesh = CreateRef<Mesh>(filePath);
        ExtractMeshData(fbxMesh, mesh);

        sdkManager->Destroy();
        return mesh;
	}

	Ref<Asset> MeshFactory::Create(const std::string& filePath)
	{
		// meshes should not be created in engine, they should be loaded from file
		ENGINE_LOG_ERROR("MeshFactory::Create not implemented");
		return nullptr;
	}

    void MeshFactory::ExtractMeshData(FbxMesh* fbxMesh, Ref<Mesh> mesh)
    {
        mesh->m_VertexCount = fbxMesh->GetControlPointsCount();
        FbxVector4* fbxVertices = fbxMesh->GetControlPoints();

        mesh->m_Vertices.reserve(mesh->m_VertexCount);
        mesh->m_Normals.reserve(mesh->m_VertexCount);
        mesh->m_UVs.reserve(mesh->m_VertexCount);

        // Extract vertices
        for (int i = 0; i < mesh->m_VertexCount; ++i)
        {
            FbxVector4 vertex = fbxVertices[i];
            mesh->m_Vertices.push_back(glm::vec3((float)vertex[0], (float)vertex[1], (float)vertex[2]));
        }

        // Extract indices
        mesh->m_Indices.clear(); // Ensure the indices vector is empty before adding new data
        int polygonCount = fbxMesh->GetPolygonCount();

        for (int i = 0; i < polygonCount; ++i)
        {
            int polygonSize = fbxMesh->GetPolygonSize(i);

            // Triangulate the polygon if it has more than 3 vertices
            if (polygonSize > 3)
            {
                for (int j = 1; j < polygonSize - 1; ++j)
                {
                    // Add indices for the two triangles forming the polygon
                    mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, 0));
                    mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, j));
                    mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, j + 1));
                }
            }
            else // Polygon is a triangle
            {
                for (int j = 0; j < polygonSize; ++j)
                {
                    mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, j));
                }
            }
        }

        //TODO:
        // Extract normals
        // Extract UVs
    }
}