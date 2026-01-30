#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
  class RenameEntityCommand : public ICommand
  {
  public:
    RenameEntityCommand(Engine::Entity entity, const std::string& name = "Renamed entity") : m_Entity(entity), m_NewName(name) {}

    void Execute() override
    {
      m_OldName = m_Entity.GetName();
      m_Entity.SetName(m_NewName);
    }

    void Undo() override
    {
      m_Entity.SetName(m_OldName);
    }

  private:
    Engine::Entity m_Entity;
    std::string m_OldName;
    std::string m_NewName;
  };
}