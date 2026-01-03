#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Window.h"
#include "Engine/Event/ApplicationEvent.h"

namespace Engine
{
  class GameInstance
  {
  public:
    GameInstance();
    ~GameInstance();

    void Initialize(Engine::Scene* scene);
    void Update();
		void OnEvent(Event& e);

    void Close();

    std::function<void()> OnExit;

  private:
    void OnWindowResize(WindowResizeEvent& e);

  private:
    bool m_Running = false;
    bool m_Minimized = false;

    Window* m_GameWindow;
  };
}