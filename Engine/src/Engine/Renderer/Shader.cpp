#include "enginepch.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"



namespace Engine 
{
	Ref<Asset> ShaderFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(path);
		default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}