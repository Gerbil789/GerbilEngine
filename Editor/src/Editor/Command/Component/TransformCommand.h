#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
  class ResetTransformComponentCommand : public ICommand
  {
  public:
    ResetTransformComponentCommand(Engine::Entity e) : m_Entity(e) {}

    void Execute() override
    {
      auto& component = m_Entity.GetComponent<Engine::TransformComponent>();
      m_Backup = component;
			component.Reset();
    }

    void Undo() override
    {
      auto& component = m_Entity.GetComponent<Engine::TransformComponent>();
      component = m_Backup;
    }

  private:
    Engine::Entity m_Entity;
    Engine::TransformComponent m_Backup;
  };

}

