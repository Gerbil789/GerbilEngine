#pragma once

#include "Engine/Asset/Asset.h"
#include <glm/glm.hpp>

namespace wgpu { class Buffer; }

namespace Engine
{
	//struct AABB
	//{
	//	glm::vec3 min = glm::vec3(FLT_MAX);
	//	glm::vec3 max = glm::vec3(-FLT_MAX);
	//};

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

		//TODO: add compile time macro to disable in export game builds
		std::vector<uint32_t> wireIndices;
	};

	class ENGINE_API Mesh : public Asset
	{
	public:
		Mesh(const MeshSpecification& specification);
		~Mesh();

		// 1. Declare Move Constructor and Move Assignment
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;

		// 2. Explicitly delete Copying (optional, but good practice for GPU resources)
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
		const wgpu::Buffer& GetVertexBuffer() const;
		const wgpu::Buffer& GetIndexBuffer() const;
		const wgpu::Buffer& GetWireIndexBuffer() const;

	private:
		std::vector<SubMesh> m_SubMeshes;

		struct MeshGPUData;
		std::unique_ptr<MeshGPUData> m_GPU;
	};
}