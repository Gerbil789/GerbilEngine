#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Math/AABB.h"
//#include <memory>
//#include <glm/glm.hpp>
#include <vector>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};

	struct SubMesh
	{
		uint32_t firstIndex;
		uint32_t indexCount;
		uint32_t materialIndex;
	};

	struct MeshSpecification
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<SubMesh> subMeshes;
		std::vector<uint32_t> wireIndices;	//TODO: add compile time macro to disable in export game builds
	};

	class ENGINE_API Mesh : public Asset
	{
	public:
		Mesh(const MeshSpecification& specification);

		const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
		const wgpu::Buffer GetVertexBuffer() const { return vertexBuffer; }
		const wgpu::Buffer GetIndexBuffer() const { return indexBuffer; }
		const wgpu::Buffer GetWireIndexBuffer() const { return wireIndexBuffer; }

		AABB aabb;

	private:
		std::vector<SubMesh> m_SubMeshes;
		wgpu::Buffer vertexBuffer;
		wgpu::Buffer indexBuffer;
		wgpu::Buffer wireIndexBuffer;
	};
}