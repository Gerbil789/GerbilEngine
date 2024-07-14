#include <Engine.h>

#include "imgui/imgui.h"

class exmapleLayer : public Engine::Layer
{
public:
	exmapleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		m_VertexArrayObject.reset(Engine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};

		std::shared_ptr<Engine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));
		Engine::BufferLayout layout = {
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArrayObject->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Engine::IndexBuffer> indexBuffer;
		indexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		m_VertexArrayObject->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			
			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";


		m_Shader.reset(new Engine::Shader(vertexSrc, fragmentSrc));


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

		for(int i = 1; i <= 5; i++)
		{
			glm::vec3 pos(i * 0.5f, 0.0f, 0.0f);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f * i));
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Engine::Renderer::Submit(m_Shader, m_VertexArrayObject, transform);
		}

		Engine::Renderer::EndScene();
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
	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::VertexArray> m_VertexArrayObject;
	Engine::OrthographicCamera m_Camera;

	float m_cameraSpeed = 1.0f;
	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
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
