#include "enginepch.h"
#include "Engine/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Engine/Renderer/Renderer2D.h"


namespace Engine
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		ENGINE_PROFILE_FUNCTION();
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();
		RenderCommand::SetViewport(0, 0, width, height);
	}


	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		ENGINE_PROFILE_FUNCTION();
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4f("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4f("u_Transform", transform);


		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);	
	}
	
}