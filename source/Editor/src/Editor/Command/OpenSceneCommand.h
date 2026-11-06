#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/SceneManager.h"

namespace editor
{
  class OpenSceneCommand : public ICommand
  {
  public:
    OpenSceneCommand(engine::SceneHandle scene) : m_scene(scene) {}

    void Execute() override
    {
      engine::SceneManager::SetActiveScene(m_scene);
    }

    void Undo() override {}

  private:
		engine::SceneHandle m_scene;
  };
}