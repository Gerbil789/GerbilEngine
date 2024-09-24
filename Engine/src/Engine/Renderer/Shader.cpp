#include "enginepch.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine 
{
	Ref<Asset> ShaderFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		ShaderSettings settings = ShaderSettings::LIGHTING; // Default settings

		try 
		{
			settings = std::any_cast<ShaderSettings>(data);
		}
		catch (const std::bad_any_cast&) 
		{
			LOG_WARNING("Failed to cast 'data' to ShaderSettings. Using default settings.");
		}

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(path, settings);
		default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}