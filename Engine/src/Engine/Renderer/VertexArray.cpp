#include "enginepch.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Engine/Core/Core.h"

namespace Engine
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
			default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}