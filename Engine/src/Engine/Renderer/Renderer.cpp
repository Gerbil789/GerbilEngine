#include "enginepch.h"
#include "Engine/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4f("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4f("u_Transform", transform);


		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);	
	}
	
}