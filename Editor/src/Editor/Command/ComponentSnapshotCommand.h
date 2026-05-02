#pragma once

#include "ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include <entt.hpp>

namespace Editor
{
  template<typename T>
  class ComponentSnapshotCommand : public ICommand
  {
  public:
    ComponentSnapshotCommand(entt::entity e, T before, T after) : m_Entity(e), m_Before(before), m_After(after) {}

    void Execute() override
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

			registry.get<T>(m_Entity) = m_After;
    }

    void Undo() override
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

			registry.get<T>(m_Entity) = m_Before;
    }

  private:
    entt::entity m_Entity;
    T m_Before, m_After;
  };
}