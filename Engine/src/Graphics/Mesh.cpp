#include "enginepch.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine
{
	struct Mesh::MeshGPUData
	{
		wgpu::Buffer vertexBuffer;
		wgpu::Buffer indexBuffer;
	};

	Mesh::Mesh(const MeshSpecification& specification)
	{
		m_GPU = std::make_unique<MeshGPUData>();

		// Vertex buffer
		wgpu::BufferDescriptor vertexBufferdesc{};
		vertexBufferdesc.label = { "VertexBuffer", WGPU_STRLEN };
		vertexBufferdesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
		vertexBufferdesc.size = specification.vertices.size() * sizeof(Vertex);
		vertexBufferdesc.mappedAtCreation = false;

		m_GPU->vertexBuffer = GraphicsContext::GetDevice().createBuffer(vertexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_GPU->vertexBuffer, 0, specification.vertices.data(), vertexBufferdesc.size);

		// Index buffer
		wgpu::BufferDescriptor indexBufferdesc{};
		indexBufferdesc.label = { "IndexBuffer", WGPU_STRLEN };
		indexBufferdesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
		indexBufferdesc.size = specification.indices.size() * sizeof(uint32_t);
		indexBufferdesc.mappedAtCreation = false;

		m_GPU->indexBuffer = GraphicsContext::GetDevice().createBuffer(indexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(m_GPU->indexBuffer, 0, specification.indices.data(), indexBufferdesc.size);

		m_SubMeshes = specification.subMeshes;
	}

	Mesh::~Mesh() = default;

	const wgpu::Buffer& Mesh::GetVertexBuffer() const
	{
		return m_GPU->vertexBuffer;
	}

	const wgpu::Buffer& Mesh::GetIndexBuffer() const
	{
		return m_GPU->indexBuffer;
	}
}