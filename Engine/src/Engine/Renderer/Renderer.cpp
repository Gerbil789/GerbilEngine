#include "enginepch.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine
{
	void Renderer::Init()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer::Flush()
	{
		ENGINE_PROFILE_FUNCTION();
	}
}