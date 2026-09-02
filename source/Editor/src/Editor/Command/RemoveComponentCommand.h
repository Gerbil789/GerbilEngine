#pragma once

#include "ICommand.h"
#include "Engine/Core/Entity.h"

namespace editor
{
  template<typename T>
  class RemoveComponentCommand : public ICommand
  {
  public:
    RemoveComponentCommand(engine::Entity entity) : m_Entity(entity)
    {
      if(m_Entity.HasComponent<T>())
      {
        m_Backup = m_Entity.GetComponent<T>();
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
				m_Entity.AddComponent<T>() = m_Backup;
      }
    }

  private:
    engine::Entity m_Entity;
    T m_Backup;
  };
}