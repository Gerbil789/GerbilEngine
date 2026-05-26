#pragma once

#include "ICommand.h"
#include <entt.hpp>

namespace Editor
{
  template<typename T>
  class AddComponentCommand : public ICommand
  {
  public:
    AddComponentCommand(entt::registry& registry, entt::entity entity, const T& initial = {}) : m_Registry(&registry), m_Entity(entity), m_Initial(initial) {}

    void Execute() override
    {
      if (!m_Registry->any_of<T>(m_Entity))
      {
        m_Registry->emplace<T>(m_Entity, m_Initial);
      }
    }

    void Undo() override
    {
      if (m_Registry->any_of<T>(m_Entity))
      {
        m_Registry->remove<T>(m_Entity);
      }
    }

  private:
    entt::registry* m_Registry;
    entt::entity m_Entity;
    T m_Initial;
  };
}