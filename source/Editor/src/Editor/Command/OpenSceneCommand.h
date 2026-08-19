#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/SceneManager.h"

namespace Editor
{
  class OpenSceneCommand : public ICommand
  {
  public:
    OpenSceneCommand(Engine::Scene scene) : m_Scene(scene) {}

    void Execute() override
    {
      Engine::SceneManager::SetActiveScene(m_Scene);
    }

    void Undo() override {}

  private:
		Engine::Scene m_Scene;
  };
}