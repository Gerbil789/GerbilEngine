#include "enginepch.h"
#include "Mesh.h"
#include "Engine/Renderer/GraphicsContext.h"

namespace Engine
{
	Mesh::Mesh(const MeshSpecification& specification)
	{
		m_IndexCount = specification.indices.size();

		auto device = GraphicsContext::GetDevice();

		// Vertex buffer
		wgpu::BufferDescriptor vertexBufferdesc{};
		vertexBufferdesc.label = { "VertexBuffer", WGPU_STRLEN };
		vertexBufferdesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
		vertexBufferdesc.size = specification.vertices.size() * sizeof(Vertex);
		vertexBufferdesc.mappedAtCreation = false;

		m_VertexBuffer = device.createBuffer(vertexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_VertexBuffer, 0, specification.vertices.data(), vertexBufferdesc.size);

		// Index buffer
		wgpu::BufferDescriptor indexBufferdesc{};
		indexBufferdesc.label = { "IndexBuffer", WGPU_STRLEN };
		indexBufferdesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
		indexBufferdesc.size = m_IndexCount * sizeof(uint16_t);
		indexBufferdesc.mappedAtCreation = false;

		m_IndexBuffer = device.createBuffer(indexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_IndexBuffer, 0, specification.indices.data(), indexBufferdesc.size);
	}
}