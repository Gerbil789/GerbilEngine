#pragma once

#include "ICommand.h"
#include "Engine/Core/SceneManager.h"
#include <entt/entity/registry.hpp>

namespace editor
{
  template<typename T>
  class RemoveComponentCommand : public ICommand
  {
  public:
    RemoveComponentCommand(entt::entity entity) : m_Entity(entity)
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();

      if(registry.any_of<T>(entity))
      {
        m_Backup = registry.get<T>(entity);
      }
    }

    void Execute() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();
      if(registry.any_of<T>(m_Entity))
      {
        registry.remove<T>(m_Entity);
      }
    }

    void Undo() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();
      if(!registry.any_of<T>(m_Entity))
      {
        registry.emplace<T>(m_Entity) = m_Backup;
      }
    }

  private:
    entt::entity m_Entity;
    T m_Backup;
  };
}