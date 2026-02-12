#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Window.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Graphics/Renderer/Renderer.h"

namespace Engine
{
  class GameInstance
  {
  public:
    GameInstance();
    ~GameInstance();

    void Initialize(Engine::Scene* scene);
    void Update(float delta);
		void OnEvent(Engine::Event& e);

    void Close();

    std::function<void()> OnExit;

  private:
    void OnWindowResize(Engine::WindowResizeEvent& event);

  private:
    bool m_Running = false;

    Engine::Window* m_GameWindow;
    Engine::Renderer m_Renderer;
		Engine::Entity m_ActiveCameraEntity;
  };
}