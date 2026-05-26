#pragma once

#include "ICommand.h"
#include <entt.hpp>

namespace Editor
{
  template<typename T>
  class RemoveComponentCommand : public ICommand
  {
  public:
    RemoveComponentCommand(entt::registry& registry, entt::entity entity) : m_Registry(&registry), m_Entity(entity)
    {


      if (m_Registry->any_of<T>(m_Entity))
      {
        m_Backup = m_Registry->get<T>(m_Entity);
      }
    }

    void Execute() override
    {
      if (m_Registry->any_of<T>(m_Entity))
      {
				m_Registry->remove<T>(m_Entity);
      }
    }

    void Undo() override
    {
      if (!m_Registry->any_of<T>(m_Entity))
      {
        m_Registry->emplace<T>(m_Entity, m_Backup);
      }
    }

  private:
    entt::registry* m_Registry;
    entt::entity m_Entity;
    T m_Backup;
  };
}