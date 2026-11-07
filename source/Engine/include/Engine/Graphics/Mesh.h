#pragma once

#include "Engine/Math/BoundingVolumeHiearchy.h"
#include <vector>
#include <filesystem>
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

	//TODO: check for optimal memory layout
	struct Mesh
	{
		AABB aabb;
		BoundingVolumeHiearchy bvh;
		std::vector<SubMesh> subMeshes;
		uint32_t materialCount = 0;
		wgpu::Buffer vertexBuffer;
		wgpu::Buffer indexBuffer;
		wgpu::Buffer wireIndexBuffer;

		[[nodiscard]] static Mesh Create(const std::filesystem::path& path);

		Mesh() = default;

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&&) noexcept = default;
		Mesh& operator=(Mesh&&) noexcept = default;
	};
}