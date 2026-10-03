#pragma once

#include "ICommand.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/SceneManager.h"
#include <entt/entity/registry.hpp>

namespace editor
{
  template<typename T>
  class ComponentSnapshotCommand : public ICommand
  {
  public:
    ComponentSnapshotCommand(entt::entity e, T before, T after) : m_Entity(e), m_Before(before), m_After(after) {}

    void Execute() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();
      registry.get<T>(m_Entity) = m_After; 
      registry.emplace_or_replace<engine::TransformDirty>(m_Entity);
    }

    void Undo() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();
      registry.get<T>(m_Entity) = m_Before;
      registry.emplace_or_replace<engine::TransformDirty>(m_Entity);
    }

  private:
    entt::entity m_Entity;
    T m_Before, m_After;
  };
}