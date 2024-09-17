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

        if (!fbxMesh->IsTriangleMesh()) 
        {
            ENGINE_LOG_ERROR("Mesh is not triangulated");
            sdkManager->Destroy();
            return nullptr;
        }

        m_Mesh = CreateRef<Mesh>(filePath);

		GetVertices(fbxMesh);
		GetIndices(fbxMesh);
		GetUVs(fbxMesh);
		GetNormals(fbxMesh);

        sdkManager->Destroy();
        return m_Mesh;
	}

    void MeshFactory::GetVertices(FbxMesh* fbxMesh)
    {
        m_Mesh->m_VertexCount = fbxMesh->GetControlPointsCount();
        m_Mesh->m_Vertices.reserve(m_Mesh->m_VertexCount);
        FbxVector4* fbxVertices = fbxMesh->GetControlPoints();

        for (int i = 0; i < m_Mesh->m_VertexCount; ++i)
        {
            FbxVector4 vertex = fbxVertices[i];
            m_Mesh->m_Vertices.push_back(glm::vec3((float)vertex[0], (float)vertex[1], (float)vertex[2]));
        }
    }

    void MeshFactory::GetIndices(FbxMesh* fbxMesh)
    {
        m_Mesh->m_PolygonCount = fbxMesh->GetPolygonCount();

		m_Mesh->m_Indices.reserve(m_Mesh->m_PolygonCount * 3);

        for (int i = 0; i < m_Mesh->m_PolygonCount; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                m_Mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, j));
            }
        }
    }

    void MeshFactory::GetNormals(FbxMesh* fbxMesh)
    {
        if (!fbxMesh) return;

        FbxGeometryElementNormal* lNormalElement = fbxMesh->GetElementNormal();
        if (lNormalElement)
        {
            if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
            {
                // Handle control point normals
                for (int lVertexIndex = 0; lVertexIndex < fbxMesh->GetControlPointsCount(); lVertexIndex++)
                {
                    int lNormalIndex = (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                        ? lVertexIndex
                        : lNormalElement->GetIndexArray().GetAt(lVertexIndex);

                    FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                    m_Mesh->m_Normals.push_back(glm::vec3((float)lNormal[0], (float)lNormal[1], (float)lNormal[2]));
                }
            }
            else if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
            {
                // Handle polygon-vertex normals
                int lIndexByPolygonVertex = 0;
                for (int lPolygonIndex = 0; lPolygonIndex < fbxMesh->GetPolygonCount(); lPolygonIndex++)
                {
                    int lPolygonSize = fbxMesh->GetPolygonSize(lPolygonIndex);
                    for (int i = 0; i < lPolygonSize; i++)
                    {
                        int lNormalIndex = (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                            ? lIndexByPolygonVertex
                            : lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

                        FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                        m_Mesh->m_Normals.push_back(glm::vec3((float)lNormal[0], (float)lNormal[1], (float)lNormal[2]));
                        lIndexByPolygonVertex++;
                    }
                }
            }
        }
    }

    void MeshFactory::GetUVs(FbxMesh* fbxMesh)
    {
        FbxStringList lUVSetNameList;
        fbxMesh->GetUVSetNames(lUVSetNameList);

        //iterating over all uv sets
        for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
        {
            //get lUVSetIndex-th uv set
            const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
            const FbxGeometryElementUV* lUVElement = fbxMesh->GetElementUV(lUVSetName);

            if (!lUVElement)
                continue;

            // only support mapping mode eByPolygonVertex and eByControlPoint
            if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
                lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
                return;

            //index array, where holds the index referenced to the uv data
            const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
            const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

            //iterating through the data by polygon
            const int lPolyCount = fbxMesh->GetPolygonCount();

            if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
            {
                for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
                {
                    // build the max index array that we need to pass into MakePoly
                    const int lPolySize = fbxMesh->GetPolygonSize(lPolyIndex);
                    for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
                    {
                        FbxVector2 lUVValue;

                        //get the index of the current vertex in control points array
                        int lPolyVertIndex = fbxMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

                        //the UV index depends on the reference mode
                        int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

                        lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
                        m_Mesh->m_UVs.push_back(glm::vec2((float)lUVValue[0], (float)lUVValue[1]));
                    }
                }
            }
            else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
            {
                int lPolyIndexCounter = 0;
                for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
                {
                    // build the max index array that we need to pass into MakePoly
                    const int lPolySize = fbxMesh->GetPolygonSize(lPolyIndex);
                    for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
                    {
                        if (lPolyIndexCounter < lIndexCount)
                        {
                            FbxVector2 lUVValue;

                            //the UV index depends on the reference mode
                            int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

                            lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

                            m_Mesh->m_UVs.push_back(glm::vec2((float)lUVValue[0], (float)lUVValue[1]));

                            lPolyIndexCounter++;
                        }
                    }
                }
            }
        }
    }
}