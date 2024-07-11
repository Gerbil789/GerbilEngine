#include <Engine.h>

#include "imgui/imgui.h"

class exmapleLayer : public Engine::Layer
{
public:
	exmapleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		//LOG_INFO("ExampleLayer - Update");

		//if (Engine::Input::IsKeyPressed(KEY_TAB))
		//	LOG_TRACE("Tab key is pressed (poll)!");
	}

	void OnEvent(Engine::Event& event) override
	{
		if(event.GetEventType() == Engine::EventType::KeyPressed)
		{
			Engine::KeyPressedEvent& e = (Engine::KeyPressedEvent&)event;
			
			if (e.GetKeyCode() == KEY_TAB)
				LOG_TRACE("Tab key is pressed (event)!");
			LOG_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

	virtual void OnImGuiRender() override
	{
		/*ImGui::Begin("BRUH");
		ImGui::Text("BRUHMAN IS HERE");
		ImGui::End();*/
	}

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
