#pragma once

#include "Engine/Core/Window.h"

class GameApp
{
public:
	GameApp();
	~GameApp();
	void Run();

private:
	Engine::Window m_Window;
	bool m_Running = true;
};








