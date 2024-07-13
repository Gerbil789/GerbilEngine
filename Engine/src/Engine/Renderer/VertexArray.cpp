#include "enginepch.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Engine
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return new OpenGLVertexArray();
			default: ENGINE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}