#pragma once

#include "Engine/Asset/Asset.h"
#include <glm/glm.hpp>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	//struct AABB
	//{
	//	glm::vec3 min = glm::vec3(FLT_MAX);
	//	glm::vec3 max = glm::vec3(-FLT_MAX);
	//};

	class Material;

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
		std::vector<uint32_t> wireframeIndices;
		std::vector<SubMesh> subMeshes;
		std::vector<std::string> materials;
	};

	class ENGINE_API Mesh : public Asset
	{
	public:
		Mesh(const MeshSpecification& specification);

		const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
		const std::vector<Material*>& GetMaterials() const { return m_Materials; }
		void SetMaterial(uint32_t slot, Material* material);

		const wgpu::Buffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const wgpu::Buffer& GetIndexBuffer() const { return m_IndexBuffer; }
		//const AABB& GetBounds() const { return m_Bounds; }

		const wgpu::Buffer& GetEdgeBuffer() const { return m_EdgeBuffer; } 

	private:
		std::vector<SubMesh> m_SubMeshes;
		std::vector<Material*> m_Materials;

		wgpu::Buffer m_VertexBuffer;
		wgpu::Buffer m_IndexBuffer;
		wgpu::Buffer m_EdgeBuffer; // for wireframe rendering
		//AABB m_Bounds;
	};
}