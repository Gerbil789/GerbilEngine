#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Window.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Graphics/Renderer/Renderer.h"

namespace Editor
{
  class GameInstance
  {
  public:
    GameInstance();
    ~GameInstance();

    void Initialize(Engine::Scene* scene);
    void Update();
		void OnEvent(Engine::Event& e);

    void Close();

    std::function<void()> OnExit;

  private:
    void OnWindowResize(Engine::WindowResizeEvent& event);

  private:
    bool m_Running = false;
    bool m_Minimized = false;

    Engine::Window* m_GameWindow;

    Engine::Renderer m_Renderer;

		Engine::Entity m_ActiveCameraEntity;
  };
}