#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
  class RemoveScriptComponentCommand : public ICommand
  {
  public:
    RemoveScriptComponentCommand(Engine::Entity e) : m_Entity(e) {}

    void Execute() override
    {
      m_Entity.RemoveComponent<Engine::ScriptComponent>();
    }

    void Undo() override
    {
      m_Entity.AddComponent<Engine::ScriptComponent>(m_Backup);
    }

  private:
    Engine::Entity m_Entity;
    Engine::ScriptComponent m_Backup;
  };

  class ResetScriptComponentCommand : public ICommand
  {
  public:
    ResetScriptComponentCommand(Engine::Entity e) : m_Entity(e) {}

    void Execute() override
    {
      auto& component = m_Entity.GetComponent<Engine::ScriptComponent>();
      m_Backup = component;
      component.instance = nullptr;
    }

    void Undo() override
    {
      auto& component = m_Entity.GetComponent<Engine::ScriptComponent>();
      component = m_Backup;
    }

  private:
    Engine::Entity m_Entity;
    Engine::ScriptComponent m_Backup;
  };

}

