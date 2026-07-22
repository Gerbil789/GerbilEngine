#include "enginepch.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine
{
	Mesh::Mesh(const MeshSpecification& specification)
	{
		// Vertex buffer
		wgpu::BufferDescriptor vertexBufferdesc;
		vertexBufferdesc.label = { "VertexBuffer", WGPU_STRLEN };
		vertexBufferdesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
		vertexBufferdesc.size = specification.vertices.size() * sizeof(Vertex);
		vertexBufferdesc.mappedAtCreation = false;

		vertexBuffer = GraphicsContext::GetDevice().createBuffer(vertexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(vertexBuffer, 0, specification.vertices.data(), vertexBufferdesc.size);

		// Index buffer
		wgpu::BufferDescriptor indexBufferdesc{};
		indexBufferdesc.label = { "IndexBuffer", WGPU_STRLEN };
		indexBufferdesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
		indexBufferdesc.size = specification.indices.size() * sizeof(uint32_t);
		indexBufferdesc.mappedAtCreation = false;

		indexBuffer = GraphicsContext::GetDevice().createBuffer(indexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(indexBuffer, 0, specification.indices.data(), indexBufferdesc.size);

		// Wireframe index buffer
		if (!specification.wireIndices.empty())
		{
			wgpu::BufferDescriptor wireIndexBufferDesc;
			wireIndexBufferDesc.label = { "WireframeIndexBuffer", WGPU_STRLEN };
			wireIndexBufferDesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
			wireIndexBufferDesc.size = specification.wireIndices.size() * sizeof(uint32_t);
			wireIndexBufferDesc.mappedAtCreation = false;
			wireIndexBuffer = GraphicsContext::GetDevice().createBuffer(wireIndexBufferDesc);
			GraphicsContext::GetQueue().writeBuffer(wireIndexBuffer, 0, specification.wireIndices.data(), wireIndexBufferDesc.size);
		}

		// Submeshes
		m_SubMeshes = specification.subMeshes;

		// AABB
		for (const auto& vertex : specification.vertices) 
		{
			aabb.min = glm::min(aabb.min, vertex.position);
			aabb.max = glm::max(aabb.max, vertex.position);
		}
	}
}