#pragma once

#include "ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include <entt.hpp>

namespace Editor
{
  template<typename T>
  class RemoveComponentCommand : public ICommand
  {
  public:
    RemoveComponentCommand(entt::entity entity) : m_Entity(entity)
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      if (registry.any_of<T>(m_Entity))
      {
        m_Backup = registry.get<T>(m_Entity);
      }
    }

    void Execute() override
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      if (registry.any_of<T>(m_Entity))
      {
				registry.remove<T>(m_Entity);
      }
    }

    void Undo() override
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      if (!registry.any_of<T>(m_Entity))
      {
        registry.emplace<T>(m_Entity, m_Backup);
      }
    }

  private:
    entt::entity m_Entity;
    T m_Backup;
  };
}