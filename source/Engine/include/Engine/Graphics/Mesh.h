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

	struct Mesh
	{
		Mesh() = default;
		Mesh(const MeshSpecification& specification);

		AABB aabb;
		std::vector<SubMesh> subMeshes;
		uint32_t materialCount = 0;
		wgpu::Buffer vertexBuffer;
		wgpu::Buffer indexBuffer;
		wgpu::Buffer wireIndexBuffer;
	};

	struct MeshHandle : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		Mesh& Get() const;

		static const MeshHandle Empty;
		static const MeshHandle Cube;
		static const MeshHandle Sphere;
	};

	inline constexpr MeshHandle MeshHandle::Empty{ 0 };
	inline constexpr MeshHandle MeshHandle::Cube{ 1 };
	inline constexpr MeshHandle MeshHandle::Sphere{ 2 };
}