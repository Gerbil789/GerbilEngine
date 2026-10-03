#pragma once

#include "ICommand.h"
#include "Engine/Core/SceneManager.h"
#include <entt/entity/registry.hpp>

namespace editor
{
  template<typename T>
  class AddComponentCommand : public ICommand
  {
  public:
    AddComponentCommand(entt::entity entity, const T& initial = {}) : m_Entity(entity), m_Initial(initial) {}

    void Execute() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();

      if(registry.any_of<T>(m_Entity))
      {
        return; // component already exists, do nothing
			}

			auto& component = registry.get<T>(m_Entity);
			component = m_Initial;
    }

    void Undo() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();

      if(registry.any_of<T>(m_Entity))
      {
        registry.remove<T>(m_Entity);
      }
    }

  private:
    entt::entity m_Entity;
    T m_Initial;
  };
}