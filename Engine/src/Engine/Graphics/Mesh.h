#pragma once

#include "Engine/Asset/Asset.h"
#include <glm/glm.hpp>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	struct AABB
	{
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);
	};

	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};

	struct MeshSpecification
	{
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;
		std::vector<uint16_t> wireframeIndices;
	};

	class Mesh : public Asset
	{
	public:
		Mesh(const MeshSpecification& specification);

		const wgpu::Buffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const wgpu::Buffer& GetIndexBuffer() const { return m_IndexBuffer; }
		uint32_t GetIndexCount() const { return m_IndexCount; }
		const AABB& GetBounds() const { return m_Bounds; }

		const wgpu::Buffer& GetEdgeBuffer() const { return m_EdgeBuffer; }
		uint32_t GetEdgeIndexCount() const { return m_EdgeIndexCount; }

	private:
		wgpu::Buffer m_VertexBuffer;
		wgpu::Buffer m_IndexBuffer;
		uint32_t m_IndexCount = 0;
		AABB m_Bounds;

		wgpu::Buffer m_EdgeBuffer;
		uint32_t m_EdgeIndexCount = 0;
	};
}