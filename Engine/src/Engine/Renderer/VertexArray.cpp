#include "enginepch.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Core.h"

namespace Engine
{
	void VertexArray::Bind() const
	{
	}
	void VertexArray::Unbind() const
	{
	}
	void Shader::SetInt(const std::string& name, int value)
	{
	}
	void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
	}
	void Shader::SetFloat(const std::string& name, float value)
	{
	}
	void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
	}
	void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
	}
	void Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
	}
	void VertexBuffer::SetLayout(const BufferLayout& layout)
	{
	}
	const BufferLayout& VertexBuffer::GetLayout() const
	{
		return m_Layout;
	}
	void VertexBuffer::SetData(const void* data, uint32_t size)
	{
		
	}
	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
	}
	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
	}
	const std::vector<Ref<VertexBuffer>>& VertexArray::GetVertexBuffers() const
	{
		// TODO: insert return statement here
		return m_VertexBuffers;
	}
	const Ref<IndexBuffer>& VertexArray::GetIndexBuffer() const
	{
		// TODO: insert return statement here
		return m_IndexBuffer;
	}
}