#include "enginepch.h"
#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine 
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
			default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count);
			default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}

}
