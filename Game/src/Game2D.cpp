#include "Game2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>



Game2D::Game2D() : Layer("Game2D"), m_CameraController(1280.f / 720.f, true) {}

void Game2D::OnAttach() 
{
	ENGINE_PROFILE_FUNCTION();
	m_Texture = Engine::Texture2D::Create("assets/textures/background.png");
	m_GerbilTexture = Engine::Texture2D::Create("assets/textures/gerbil.jpg");
	m_TileTexture = Engine::Texture2D::Create("assets/textures/Tile.png");
}

void Game2D::OnDetach() 
{
	ENGINE_PROFILE_FUNCTION();
}

void Game2D::OnUpdate(Engine::Timestep ts) 
{
	ENGINE_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);
	
	{
		ENGINE_PROFILE_SCOPE("Renderer::Clear");
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;


		ENGINE_PROFILE_SCOPE("Renderer2D::BeginScene");
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		//Background
		Engine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture, 5.0f, Engine::Color::LightGray());

		Engine::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f }, { 1.0f, 1.0f }, glm::radians(45.0f), Engine::Color::Yellow());
		Engine::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.5f, 0.5f }, Engine::Color::Red());
		Engine::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.5f, 0.5f }, Engine::Color::Green());
		Engine::Renderer2D::DrawQuad({ 1.0f, 0.0f }, { 0.5f, 0.5f }, Engine::Color::Blue());



		for (int i = -5; i <= 5; i++)
			for (int j = -5; j <= 5; j++)
				Engine::Renderer2D::DrawQuad({ (float)i / 2, (float)j / 2, 0.2f}, {0.5f, 0.5f}, m_TileTexture, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));

		Engine::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f, 0.3f }, { 1.0f, 1.0f }, glm::radians(rotation), m_GerbilTexture, 1.0f, Engine::Color::Radnom());



		Engine::Renderer2D::EndScene();

	}


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
	//auto shader = m_ShaderLibrary.Get("FlatColor");

	//ImGui::Begin("Profiler");
	/*if (ImGui::ColorEdit4("Color", glm::value_ptr(m_Color))) {
		shader->SetFloat4("u_Color", m_Color);
	}*/







	//ImGui::End();


}