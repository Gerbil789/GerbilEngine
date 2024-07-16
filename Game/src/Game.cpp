#include <Engine.h>
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

class exmapleLayer : public Engine::Layer
{
public:
	exmapleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
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
		std::dynamic_pointer_cast<Engine::OpenGLShader>(shader)->SetUniformFloat3("u_Color", m_Color);
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		//LOG_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());

		if(Engine::Input::IsKeyPressed(KEY_A))
			m_CameraPosition.x += m_cameraSpeed * ts;
		else if (Engine::Input::IsKeyPressed(KEY_D))
			m_CameraPosition.x -= m_cameraSpeed * ts;


		if (Engine::Input::IsKeyPressed(KEY_W))
			m_CameraPosition.y -= m_cameraSpeed * ts;
		else if (Engine::Input::IsKeyPressed(KEY_S))
			m_CameraPosition.y += m_cameraSpeed * ts;


		m_Camera.SetPosition(m_CameraPosition);

		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_Camera);

		auto shader = m_ShaderLibrary.Get("Texture");
		
		m_Texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		Engine::Renderer::Submit(shader, m_VertexArrayObject, transform);


		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		auto shader = m_ShaderLibrary.Get("Texture");

		ImGui::Begin("Settings");
		if (ImGui::ColorEdit3("Color", glm::value_ptr(m_Color))) {
			std::dynamic_pointer_cast<Engine::OpenGLShader>(shader)->SetUniformFloat3("u_Color", m_Color);
		}

		ImGui::End();
	}

	void OnEvent(Engine::Event& event) override
	{
		Engine::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Engine::KeyPressedEvent>(ENGINE_BIND_EVENT_FN(exmapleLayer::OnKeyPressedEvent));

	}
private:
	bool OnKeyPressedEvent(Engine::KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == KEY_LEFT)
		{
			m_CameraPosition.x += m_cameraSpeed;
		}
		else if (event.GetKeyCode() == KEY_RIGHT)
		{
			m_CameraPosition.x -= m_cameraSpeed;
		}
		else if (event.GetKeyCode() == KEY_UP)
		{
			m_CameraPosition.y -= m_cameraSpeed;
		}
		else if (event.GetKeyCode() == KEY_DOWN)
		{
			m_CameraPosition.y += m_cameraSpeed;
		}
		return false;
	}

	
private:
	Engine::ShaderLibrary m_ShaderLibrary;

	//Engine::Ref<Engine::Shader> m_Shader;
	Engine::Ref<Engine::VertexArray> m_VertexArrayObject;

	Engine::Ref<Engine::Texture2D> m_Texture;


	Engine::OrthographicCamera m_Camera;

	float m_cameraSpeed = 1.0f;
	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };

	glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };
};



class Game : public Engine::Application
{
public:
	Game()
	{
		PushLayer(new exmapleLayer());
	}

	~Game()
	{

	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Game();
}
