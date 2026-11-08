#pragma once

#include "ICommand.h"
#include "Engine/Core/Context.h"
#include <entt/entity/registry.hpp>

namespace editor
{
  template<typename T>
  class RemoveComponentCommand : public ICommand
  {
  public:
    RemoveComponentCommand(entt::entity entity) : m_entity(entity)
    {
      entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;

      if(registry.any_of<T>(entity))
      {
        m_backup = registry.get<T>(entity);
      }
    }

    void Execute() override
    {
      entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;
      if(registry.any_of<T>(m_entity))
      {
        registry.remove<T>(m_entity);
      }
    }

    void Undo() override
    {
      entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;
      if(!registry.any_of<T>(m_entity))
      {
        registry.emplace<T>(m_entity) = m_backup;
      }
    }

  private:
    entt::entity m_entity;
    T m_backup;
  };
}