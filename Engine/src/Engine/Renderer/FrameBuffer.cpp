#include "enginepch.h"
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"


namespace Engine {

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLFrameBuffer>(spec);
			default: ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}
}