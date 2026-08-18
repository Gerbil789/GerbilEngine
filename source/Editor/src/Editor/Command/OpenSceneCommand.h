#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/SceneManager.h"

namespace Editor
{
  class OpenSceneCommand : public ICommand
  {
  public:
    OpenSceneCommand(Engine::Uuid id) : m_SceneId(id) {}

    void Execute() override
    {
      Engine::SceneManager::SetActiveScene(m_SceneId);
    }

    void Undo() override {}

  private:
		Engine::Uuid m_SceneId;
  };
}