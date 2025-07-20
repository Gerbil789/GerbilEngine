#pragma once

#include "Engine/Core/Asset.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Mesh : public Asset
	{
	public:
		Mesh(const std::filesystem::path& path);

		const wgpu::Buffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const wgpu::Buffer& GetIndexBuffer() const { return m_IndexBuffer; }
		uint32_t GetIndexCount() const { return m_IndexCount; }

	private:
		wgpu::Buffer m_VertexBuffer;
		wgpu::Buffer m_IndexBuffer;
		uint32_t m_IndexCount = 0;
	};
}