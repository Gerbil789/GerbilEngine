#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/SelectionManager.h"

namespace Editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(entt::entity entity) 
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      if (entity != entt::null)
      {
        m_Name = registry.get<Engine::NameComponent>(entity).name;
        m_Entity = entity;
			}
    } 

    void Execute() override
    {
      if (m_Entity == entt::null) return;

      SelectionManager::Clear(SelectionType::Entity);
      Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
      entt::registry& registry = scene.GetRegistry();
			registry.destroy(m_Entity);
    }

    void Undo() override
    {
      Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
      entt::registry& registry = scene.GetRegistry();

      m_Entity = scene.CreateEntity(m_Name);
			auto id = registry.get<Engine::IdentityComponent>(m_Entity).id;

			SelectionManager::Select(SelectionType::Entity, id);
    }

  private:
    std::string m_Name;
    entt::entity m_Entity;
  };
}