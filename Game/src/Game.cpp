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

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;	

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			uniform vec3 u_Color;
			uniform sampler2D u_Texture;
			in vec2 v_TexCoord;	


			void main()
			{
				color = texture(u_Texture, v_TexCoord) * vec4(u_Color, 1.0);
			}
		)";


		m_Shader.reset(Engine::Shader::Create(vertexSrc, fragmentSrc));

		m_Texture = Engine::Texture2D::Create("assets/textures/gerbil.jpg");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->SetUniformInt("u_Texture", 0);
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->SetUniformFloat3("u_Color", m_Color);
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


	
		//for(int i = 1; i <= 5; i++)
		//{
		//	glm::vec3 pos(i * 0.5f, 0.0f, 0.0f);
		//	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f * i));
		//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
		//	m_Texture->Bind();
		//	Engine::Renderer::Submit(m_Shader, m_VertexArrayObject, transform);
		//}

		
		m_Texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		Engine::Renderer::Submit(m_Shader, m_VertexArrayObject, transform);




		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		if (ImGui::ColorEdit3("Color", glm::value_ptr(m_Color))) {
			std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->SetUniformFloat3("u_Color", m_Color);
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
	Engine::Ref<Engine::Shader> m_Shader;
	Engine::Ref<Engine::VertexArray> m_VertexArrayObject;

	Engine::Ref<Engine::Texture2D> m_Texture;


	Engine::OrthographicCamera m_Camera;

	float m_cameraSpeed = 1.0f;
	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };

	glm::vec3 m_Color = { .8f, .3f, .2f };
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
