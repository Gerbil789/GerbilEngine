#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/SceneManager.h"

namespace editor
{
  class OpenSceneCommand : public ICommand
  {
  public:
    OpenSceneCommand(engine::SceneHandle scene) : m_Scene(scene) {}

    void Execute() override
    {
      engine::SceneManager::SetActiveScene(m_Scene);
    }

    void Undo() override {}

  private:
		engine::SceneHandle m_Scene;
  };
}