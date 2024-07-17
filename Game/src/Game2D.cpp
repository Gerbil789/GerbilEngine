#include "Game2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>




Game2D::Game2D() : Layer("Game2D"), m_CameraController(1280.f / 720.f, true)
{
	m_VertexArrayObject.reset(Engine::VertexArray::Create());

	float vertices[4 * 5] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f ,0.0f, 1.0f
	};

	std::shared_ptr<Engine::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));
	Engine::BufferLayout layout = {
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float2, "a_TexCoord" }
	};

	vertexBuffer->SetLayout(layout);
	m_VertexArrayObject->AddVertexBuffer(vertexBuffer);

	uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
	std::shared_ptr<Engine::IndexBuffer> indexBuffer;
	indexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

	m_VertexArrayObject->SetIndexBuffer(indexBuffer);

	auto shader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
	m_Texture = Engine::Texture2D::Create("assets/textures/Tile.png");

	std::dynamic_pointer_cast<Engine::OpenGLShader>(shader)->Bind();
	std::dynamic_pointer_cast<Engine::OpenGLShader>(shader)->SetUniformInt("u_Texture", 0);
	std::dynamic_pointer_cast<Engine::OpenGLShader>(shader)->SetUniformFloat4("u_Color", m_Color);
}

void Game2D::OnAttach() 
{

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

	Engine::Renderer::BeginScene(m_CameraController.GetCamera());

	auto shader = m_ShaderLibrary.Get("Texture");

	m_Texture->Bind();
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Engine::Renderer::Submit(shader, m_VertexArrayObject, transform);


	Engine::Renderer::EndScene();
}

void Game2D::OnEvent(Engine::Event& e) 
{
	m_CameraController.OnEvent(e);
}

void Game2D::OnImGuiRender() 
{
	auto shader = m_ShaderLibrary.Get("Texture");

	ImGui::Begin("Settings");
	if (ImGui::ColorEdit4("Color", glm::value_ptr(m_Color))) {
		std::dynamic_pointer_cast<Engine::OpenGLShader>(shader)->SetUniformFloat4("u_Color", m_Color);
	}

	ImGui::End();
}