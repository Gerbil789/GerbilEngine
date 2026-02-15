#pragma once

#include "ICommand.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
  template<typename T>
  class RemoveComponentCommand : public ICommand
  {
  public:
    RemoveComponentCommand(Engine::Entity entity) : m_Entity(entity)
    {
      if (entity.HasComponent<T>())
      {
        m_Backup = entity.GetComponent<T>();
      }
    }

    void Execute() override
    {
      if (m_Entity.HasComponent<T>())
      {
        m_Entity.RemoveComponent<T>();
      }

    }

    void Undo() override
    {
      if (!m_Entity.HasComponent<T>())
      {
        m_Entity.AddComponent<T>(m_Backup);
      }
    }

  private:
    Engine::Entity m_Entity;
    T m_Backup;
  };
}