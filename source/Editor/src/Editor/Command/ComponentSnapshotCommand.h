#pragma once

#include "ICommand.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Context.h"
#include <entt/entity/registry.hpp>

namespace editor
{
  template<typename T>
  class ComponentSnapshotCommand : public ICommand
  {
  public:
    ComponentSnapshotCommand(entt::entity e, T before, T after) : m_entity(e), m_before(before), m_after(after) {}

    void Execute() override
    {
      entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;
      registry.get<T>(m_entity) = m_after; 
      registry.emplace_or_replace<engine::tag::TransformDirty>(m_entity);
    }

    void Undo() override
    {
      entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;
      registry.get<T>(m_entity) = m_before;
      registry.emplace_or_replace<engine::tag::TransformDirty>(m_entity);
    }

  private:
    entt::entity m_entity;
    T m_before, m_after;
  };
}