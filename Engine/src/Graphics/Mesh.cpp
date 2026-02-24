#include "enginepch.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Material.h"

namespace Engine
{
	Mesh::Mesh(const MeshSpecification& specification)
	{
		// Calculate AABB bounds
		//for (const Vertex& v : specification.vertices)
		//{
		//	m_Bounds.min = glm::min(m_Bounds.min, v.position);
		//	m_Bounds.max = glm::max(m_Bounds.max, v.position);
		//}

		// Vertex buffer
		wgpu::BufferDescriptor vertexBufferdesc{};
		vertexBufferdesc.label = { "VertexBuffer", WGPU_STRLEN };
		vertexBufferdesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
		vertexBufferdesc.size = specification.vertices.size() * sizeof(Vertex);
		vertexBufferdesc.mappedAtCreation = false;

		m_VertexBuffer = GraphicsContext::GetDevice().createBuffer(vertexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_VertexBuffer, 0, specification.vertices.data(), vertexBufferdesc.size);

		// Index buffer
		wgpu::BufferDescriptor indexBufferdesc{};
		indexBufferdesc.label = { "IndexBuffer", WGPU_STRLEN };
		indexBufferdesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
		indexBufferdesc.size = specification.indices.size() * sizeof(uint32_t);
		indexBufferdesc.mappedAtCreation = false;

		m_IndexBuffer = GraphicsContext::GetDevice().createBuffer(indexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_IndexBuffer, 0, specification.indices.data(), indexBufferdesc.size);

		// Edge buffer for wireframe rendering
		wgpu::BufferDescriptor edgeBufferdesc{};
		edgeBufferdesc.label = { "EdgeBuffer", WGPU_STRLEN };
		edgeBufferdesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
		edgeBufferdesc.size = specification.wireframeIndices.size() * sizeof(uint32_t);
		edgeBufferdesc.mappedAtCreation = false;

		m_EdgeBuffer = GraphicsContext::GetDevice().createBuffer(edgeBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_EdgeBuffer, 0, specification.wireframeIndices.data(), edgeBufferdesc.size);	

		m_SubMeshes = specification.subMeshes;

		// Materials
		if (!specification.materials.empty())
		{
			m_Materials.resize(specification.materials.size(), nullptr);

			for(int i = 0; i < specification.materials.size(); ++i)
			{
				m_Materials[i] = Materials::GetDefault();
			}
		}
		else
		{
			m_Materials.push_back(Materials::GetDefault());
			for (SubMesh& sub : m_SubMeshes)
			{
				sub.materialIndex = 0;
			}
		}




	}
	void Mesh::SetMaterial(uint32_t slot, Material* material)
	{
		if (slot < m_Materials.size())
		{
			m_Materials[slot] = material;
		}
	}
}