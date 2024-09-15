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

        m_Mesh = CreateRef<Mesh>(filePath);

		GetVertices(fbxMesh);
		GetIndices(fbxMesh);
		GetUVs(fbxMesh);
		GetNormals(rootNode);

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
        int polygonCount = fbxMesh->GetPolygonCount();
		m_Mesh->m_Indices.reserve(polygonCount * 3);

        for (int i = 0; i < polygonCount; ++i)
        {
            int polygonSize = fbxMesh->GetPolygonSize(i);

            // Triangulate the polygon if it has more than 3 vertices
            if (polygonSize > 3)
            {
                for (int j = 1; j < polygonSize - 1; ++j)
                {
                    // Add indices for the two triangles forming the polygon
                    m_Mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, 0));
                    m_Mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, j));
                    m_Mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, j + 1));
                }
            }
            else // Polygon is a triangle
            {
                for (int j = 0; j < polygonSize; ++j)
                {
                    m_Mesh->m_Indices.push_back(fbxMesh->GetPolygonVertex(i, j));
                }
            }
        }
    }

    void MeshFactory::GetNormals(FbxNode* pNode)
    {
        if (!pNode) return;
            
        //get mesh
        FbxMesh* lMesh = pNode->GetMesh();
        if (lMesh)
        {
            //print mesh node name
            FBXSDK_printf("current mesh node: %s\n", pNode->GetName());

            //get the normal element
            FbxGeometryElementNormal* lNormalElement = lMesh->GetElementNormal();
            if (lNormalElement)
            {
                //mapping mode is by control points. The mesh should be smooth and soft.
                //we can get normals by retrieving each control point
                if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
                {
                    //Let's get normals of each vertex, since the mapping mode of normal element is by control point
                    for (int lVertexIndex = 0; lVertexIndex < lMesh->GetControlPointsCount(); lVertexIndex++)
                    {
                        int lNormalIndex = 0;
                        //reference mode is direct, the normal index is same as vertex index.
                        //get normals by the index of control vertex
                        if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                            lNormalIndex = lVertexIndex;

                        //reference mode is index-to-direct, get normals by the index-to-direct
                        if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                            lNormalIndex = lNormalElement->GetIndexArray().GetAt(lVertexIndex);

                        //Got normals of each vertex.
                        FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                        //FBXSDK_printf("normals for vertex[%d]: %f %f %f %f \n", lVertexIndex, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);

                        m_Mesh->m_Normals.push_back(glm::vec3((float)lNormal[0], (float)lNormal[1], (float)lNormal[2]));
                    }//end for lVertexIndex
                }//end eByControlPoint
                //mapping mode is by polygon-vertex.
                //we can get normals by retrieving polygon-vertex.
                else if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                {
                    int lIndexByPolygonVertex = 0;
                    //Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
                    for (int lPolygonIndex = 0; lPolygonIndex < lMesh->GetPolygonCount(); lPolygonIndex++)
                    {
                        //get polygon size, you know how many vertices in current polygon.
                        int lPolygonSize = lMesh->GetPolygonSize(lPolygonIndex);
                        //retrieve each vertex of current polygon.
                        for (int i = 0; i < lPolygonSize; i++)
                        {
                            int lNormalIndex = 0;
                            //reference mode is direct, the normal index is same as lIndexByPolygonVertex.
                            if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                                lNormalIndex = lIndexByPolygonVertex;

                            //reference mode is index-to-direct, get normals by the index-to-direct
                            if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                                lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

                            //Got normals of each polygon-vertex.
                            FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                            //FBXSDK_printf("normals for polygon[%d]vertex[%d]: %f %f %f %f \n", lPolygonIndex, i, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);

                            m_Mesh->m_Normals.push_back(glm::vec3((float)lNormal[0], (float)lNormal[1], (float)lNormal[2]));
                            lIndexByPolygonVertex++;
                        }//end for i //lPolygonSize
                    }//end for lPolygonIndex //PolygonCount

                }//end eByPolygonVertex
            }//end if lNormalElement

        }//end if lMesh

        //recursively traverse each node in the scene
        int i, lCount = pNode->GetChildCount();
        for (i = 0; i < lCount; i++)
        {
            GetNormals(pNode->GetChild(i));
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