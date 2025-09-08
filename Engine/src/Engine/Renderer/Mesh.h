#pragma once

#include "Engine/Asset/Asset.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
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
	};

	class Mesh : public Asset
	{
	public:
		Mesh(const MeshSpecification& specification);

		const wgpu::Buffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const wgpu::Buffer& GetIndexBuffer() const { return m_IndexBuffer; }
		uint32_t GetIndexCount() const { return m_IndexCount; }

	private:
		wgpu::Buffer m_VertexBuffer;
		wgpu::Buffer m_IndexBuffer;
		uint32_t m_IndexCount = 0;
	};
}