#include "enginepch.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace engine
{
	MeshAsset& Mesh::Get() const
	{
		return AssetManager::GetAsset(*this);
	}

	MeshAsset::MeshAsset(const MeshSpecification& specification)
	{
		// Vertex buffer
		{
			wgpu::BufferDescriptor desc;
			desc.label = "VertexBuffer";
			desc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
			desc.size = specification.vertices.size() * sizeof(Vertex);
			desc.mappedAtCreation = false;

			vertexBuffer = GraphicsContext::GetDevice().CreateBuffer(&desc);
			GraphicsContext::GetQueue().WriteBuffer(vertexBuffer, 0, specification.vertices.data(), desc.size);
		}

		// Index buffer
		{
			wgpu::BufferDescriptor desc;
			desc.label = "IndexBuffer";
			desc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
			desc.size = specification.indices.size() * sizeof(uint32_t);
			desc.mappedAtCreation = false;

			indexBuffer = GraphicsContext::GetDevice().CreateBuffer(&desc);
			GraphicsContext::GetQueue().WriteBuffer(indexBuffer, 0, specification.indices.data(), desc.size);
		}

		// Wireframe index buffer
		if (!specification.wireIndices.empty())
		{
			wgpu::BufferDescriptor wireIndexBufferDesc;
			wireIndexBufferDesc.label = "WireframeIndexBuffer";
			wireIndexBufferDesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
			wireIndexBufferDesc.size = specification.wireIndices.size() * sizeof(uint32_t);
			wireIndexBufferDesc.mappedAtCreation = false;
			wireIndexBuffer = GraphicsContext::GetDevice().CreateBuffer(&wireIndexBufferDesc);
			GraphicsContext::GetQueue().WriteBuffer(wireIndexBuffer, 0, specification.wireIndices.data(), wireIndexBufferDesc.size);
		}

		// Submeshes
		subMeshes = specification.subMeshes;

		materialCount = subMeshes.empty() ? 0 : std::ranges::max_element(subMeshes, {}, &SubMesh::materialIndex)->materialIndex + 1;

		// AABB
		for (const auto& vertex : specification.vertices)
		{
			aabb.min = glm::min(aabb.min, vertex.position);
			aabb.max = glm::max(aabb.max, vertex.position);
		}
	}
}