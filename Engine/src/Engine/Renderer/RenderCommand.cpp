#include "enginepch.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine 
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}