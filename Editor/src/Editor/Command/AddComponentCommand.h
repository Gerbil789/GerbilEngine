#pragma once

#include "ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include <entt.hpp>

namespace Editor
{
  template<typename T>
  class AddComponentCommand : public ICommand
  {
  public:
    AddComponentCommand(entt::entity entity, const T& initial = {}) : m_Entity(entity), m_Initial(initial) {}

    void Execute() override
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      if (!registry.any_of<T>(m_Entity))
      {
        registry.emplace<T>(m_Entity, m_Initial);
      }
    }

    void Undo() override
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      if (registry.any_of<T>(m_Entity))
      {
        registry.remove<T>(m_Entity);
      }
    }

  private:
    entt::entity m_Entity;
    T m_Initial;
  };
}