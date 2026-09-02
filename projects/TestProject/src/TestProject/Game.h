#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Graphics/Renderer/Renderer.h"

class Game
{
public:
  Game();
  ~Game();

private:
  void Update();

private:
  engine::Window m_Window;
  engine::Renderer m_Renderer;
  bool m_Running = true;
};