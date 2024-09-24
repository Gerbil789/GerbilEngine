#include "enginepch.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"


namespace Engine
{
	Ref<Asset> Texture2DFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		return Create(path);
	}

	Ref<Asset> Texture2DFactory::Create(const std::filesystem::path& path, const std::any& data)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
		default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}

	Ref<Asset> Texture2DFactory::CreateTexture(uint32_t width, uint32_t height, uint32_t data)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: 
		{
			Ref<Texture2D> texture = CreateRef<OpenGLTexture2D>(width, height);
			texture->SetData(&data, sizeof(uint32_t));
			return texture;
		}
		default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}