#include <Engine.h>


class exmapleLayer : public Engine::Layer
{
public:
	exmapleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		//LOG_INFO("ExampleLayer - Update");
	}

	void OnEvent(Engine::Event& event) override
	{
		LOG_TRACE("{0}", event.ToString());
	}

};



class Game : public Engine::Application
{
public:
	Game()
	{
		PushLayer(new exmapleLayer());
		PushOverlay(new Engine::ImGuiLayer());
	}

	~Game()
	{

	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Game();
}
