#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Math/AABB.h"
#include <vector>
#include <webgpu/webgpu_cpp.h>

namespace engine
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

	struct MeshAsset
	{
		MeshAsset() = default;
		MeshAsset(const MeshSpecification& specification);

		AABB aabb;
		std::vector<SubMesh> subMeshes;
		uint32_t materialCount = 0;
		wgpu::Buffer vertexBuffer;
		wgpu::Buffer indexBuffer;
		wgpu::Buffer wireIndexBuffer;
	};


	struct Mesh : public AssetHandle
	{
		using AssetHandle::AssetHandle;

		MeshAsset& Get() const;

		const std::vector<SubMesh>& GetSubMeshes() const;
		uint32_t GetMaterialCount() const;
		const wgpu::Buffer GetVertexBuffer() const;
		const wgpu::Buffer GetIndexBuffer() const;
		const wgpu::Buffer GetWireIndexBuffer() const;
		const AABB& GetAABB() const;
	};
}