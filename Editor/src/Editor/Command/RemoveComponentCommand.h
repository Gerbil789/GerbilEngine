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
      if (entity.Has<T>())
      {
        m_Backup = entity.Get<T>();
      }
    }

    void Execute() override
    {
      if (m_Entity.Has<T>())
      {
        m_Entity.Remove<T>();
      }

    }

    void Undo() override
    {
      if (!m_Entity.Has<T>())
      {
        m_Entity.Add<T>(m_Backup);
      }
    }

  private:
    Engine::Entity m_Entity;
    T m_Backup;
  };
}