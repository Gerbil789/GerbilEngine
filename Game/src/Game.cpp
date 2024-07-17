#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "Game2D.h"

class Game : public Engine::Application
{
public:
	Game()
	{
		PushLayer(new Game2D());
	}

	~Game()
	{

	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Game();
}
