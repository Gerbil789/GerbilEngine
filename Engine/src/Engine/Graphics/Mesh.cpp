#include "enginepch.h"
#include "Mesh.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine
{
	Mesh::Mesh(const MeshSpecification& specification)
	{
		m_IndexCount = static_cast<uint32_t>(specification.indices.size());

		// Calculate AABB bounds
		for (const Vertex& v : specification.vertices)
		{
			m_Bounds.min = glm::min(m_Bounds.min, v.position);
			m_Bounds.max = glm::max(m_Bounds.max, v.position);
		}

		// Vertex buffer
		wgpu::BufferDescriptor vertexBufferdesc{};
		vertexBufferdesc.label = { "VertexBuffer" };
		vertexBufferdesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
		vertexBufferdesc.size = specification.vertices.size() * sizeof(Vertex);
		vertexBufferdesc.mappedAtCreation = false;

		m_VertexBuffer = GraphicsContext::GetDevice().createBuffer(vertexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_VertexBuffer, 0, specification.vertices.data(), vertexBufferdesc.size);

		// Index buffer
		wgpu::BufferDescriptor indexBufferdesc{};
		indexBufferdesc.label = { "IndexBuffer" };
		indexBufferdesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
		indexBufferdesc.size = m_IndexCount * sizeof(uint16_t);
		indexBufferdesc.mappedAtCreation = false;

		m_IndexBuffer = GraphicsContext::GetDevice().createBuffer(indexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_IndexBuffer, 0, specification.indices.data(), indexBufferdesc.size);
	}
}