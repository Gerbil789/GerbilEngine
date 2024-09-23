#pragma once

#include "Engine/Core/Asset.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Mesh : public Asset
	{
	public:
		Mesh(const std::filesystem::path& path) : Asset(path) {}

		int GetVertexCount() const { return m_VertexCount; }
		int GetPolygonCount() const { return m_PolygonCount; }
		const std::vector<glm::vec3>& GetVertices() const { return m_Vertices; }
		const std::vector< uint32_t>& GetIndices() const { return m_Indices; }
		const std::vector<glm::vec3>& GetNormals() const { return m_Normals; }
		const std::vector<glm::vec2>& GetUVs() const { return m_UVs; }

		friend class MeshFactory;
	private:
		int m_VertexCount = 0;
		int m_PolygonCount = 0;

		std::vector<glm::vec3> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<glm::vec3> m_Normals;
		std::vector<glm::vec2> m_UVs;
	};

	class MeshFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path) override;
		virtual Ref<Asset> Create(const std::filesystem::path& path) { return nullptr; } // Not implemented, meshes are not created, only loaded
	};
}