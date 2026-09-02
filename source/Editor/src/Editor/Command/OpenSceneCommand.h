#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/SceneManager.h"

namespace editor
{
  class OpenSceneCommand : public ICommand
  {
  public:
    OpenSceneCommand(engine::Scene scene) : m_Scene(scene) {}

    void Execute() override
    {
      engine::SceneManager::SetActiveScene(m_Scene);
    }

    void Undo() override {}

  private:
		engine::Scene m_Scene;
  };
}