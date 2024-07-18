#include "Game2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>


template<typename Fn>

class Timer 
{
public:
	Timer(const char* name, Fn&& func): m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	bool m_Stopped;

	Fn m_Func;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })

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
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	
	}

	{
		PROFILE_SCOPE("Renderer2D::ResetStats");
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();
	
	}

	{
		PROFILE_SCOPE("Renderer2D::BeginScene");
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 5.0f, 5.0f }, m_Texture);

		Engine::Renderer2D::DrawQuad({ -0.75f, 0.0f }, { 1.0f, 1.0f }, glm::vec4(0.2f, 0.3f, 0.8f, 1.0f));

		Engine::Renderer2D::DrawQuad({ 0.75f, 0.0f }, { 1.0f, 1.0f }, glm::vec4(0.8f, 0.3f, 0.2f, 1.0f));


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

	ImGui::Begin("Profiler");
	/*if (ImGui::ColorEdit4("Color", glm::value_ptr(m_Color))) {
		shader->SetFloat4("u_Color", m_Color);
	}*/



	for(auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, "%.3fms ");
		strcat(label, result.Name);
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();



	ImGui::End();


}