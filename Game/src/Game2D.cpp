#include "Game2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>


Game2D::Game2D() : Layer("Game2D"), m_CameraController(1280.f / 720.f, true) {}

void Game2D::OnAttach() 
{
	m_Texture = Engine::Texture2D::Create("assets/textures/background.png");
}

void Game2D::OnDetach() 
{

}

void Game2D::OnUpdate(Engine::Timestep ts) 
{
	//LOG_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());

	m_CameraController.OnUpdate(ts);


	Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Engine::RenderCommand::Clear();


	Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Engine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 5.0f, 5.0f }, m_Texture);

	Engine::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_Color);

	Engine::Renderer2D::DrawQuad({ 1.0f, 0.5f }, { 0.5f, 2.0f }, glm::vec4(0.8f, 0.3f, 0.2f, 1.0f));



	Engine::Renderer2D::EndScene();

	/*Engine::Renderer::BeginScene(m_CameraController.GetCamera());

	auto shader = m_ShaderLibrary.Get("Texture");

	m_Texture->Bind();
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Engine::Renderer::Submit(shader, m_VertexArrayObject, transform);


	Engine::Renderer::EndScene();*/
}

void Game2D::OnEvent(Engine::Event& e) 
{
	m_CameraController.OnEvent(e);
}

void Game2D::OnImGuiRender() 
{
	auto shader = m_ShaderLibrary.Get("FlatColor");

	ImGui::Begin("Settings");
	if (ImGui::ColorEdit4("Color", glm::value_ptr(m_Color))) {
		shader->SetFloat4("u_Color", m_Color);
	}

	ImGui::End();
}