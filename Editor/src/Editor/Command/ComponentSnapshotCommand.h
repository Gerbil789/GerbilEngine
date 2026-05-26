#pragma once

#include "ICommand.h"
#include <entt.hpp>

namespace Editor
{
  template<typename T>
  class ComponentSnapshotCommand : public ICommand
  {
  public:
    ComponentSnapshotCommand(entt::registry& registry, entt::entity e, T before, T after) : m_Registry(&registry), m_Entity(e), m_Before(before), m_After(after) {}

    void Execute() override
    {
			m_Registry->get<T>(m_Entity) = m_After;
    }

    void Undo() override
    {
			m_Registry->get<T>(m_Entity) = m_Before;
    }

  private:
    entt::registry* m_Registry;
    entt::entity m_Entity;
    T m_Before, m_After;
  };
}