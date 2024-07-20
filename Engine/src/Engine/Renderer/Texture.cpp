#include "enginepch.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"


namespace Engine
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
		default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}


	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
		default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}