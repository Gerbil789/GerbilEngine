#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine
{
	class VertexArray
	{
	public:
		VertexArray() = default;
		~VertexArray() {}

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const;
		const Ref<IndexBuffer>& GetIndexBuffer() const;

	private:
		uint32_t m_RendererID = 0;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}