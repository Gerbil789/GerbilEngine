#pragma once

#include "Engine/Core/Asset.h"

#include <glm/glm.hpp>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Mesh : public Asset
	{
	public:
		Mesh(const std::filesystem::path& path, wgpu::Buffer vertexBuffer, wgpu::Buffer indexBuffer, uint32_t indexCount) : Asset(path), m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_IndexCount(indexCount) {}

		const wgpu::Buffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const wgpu::Buffer& GetIndexBuffer() const { return m_IndexBuffer; }
		uint32_t GetIndexCount() const { return m_IndexCount; }

	private:
		wgpu::Buffer m_VertexBuffer;
		wgpu::Buffer m_IndexBuffer;
		uint32_t m_IndexCount = 0;
	};

	class MeshFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data) override;
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data) { return nullptr; } // Not implemented, meshes are not created, only loaded
	};
}