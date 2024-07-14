#include "enginepch.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine 
{
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLShader(vertexSrc, fragmentSrc);
		default: ENGINE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}