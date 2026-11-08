#pragma once

#include "ICommand.h"
#include "Engine/Core/Context.h"
#include <entt/entity/registry.hpp>

namespace editor
{
  template<typename T>
  class AddComponentCommand : public ICommand
  {
  public:
    AddComponentCommand(entt::entity entity, const T& initial = {}) : m_entity(entity), m_initial(initial) {}

    void Execute() override
    {
      entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;

      if(registry.any_of<T>(m_entity))
      {
        return; // component already exists, do nothing
			}

			auto& component = registry.get<T>(m_entity);
			component = m_initial;
    }

    void Undo() override
    {
      entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;

      if(registry.any_of<T>(m_entity))
      {
        registry.remove<T>(m_entity);
      }
    }

  private:
    entt::entity m_entity;
    T m_initial;
  };
}