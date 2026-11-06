#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/SceneManager.h"

namespace editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(entt::entity entity) : m_entity(entity)
    {
      if (entity == entt::null) return;

      entt::registry& registry = engine::SceneManager::GetActiveScene().Get().registry;
      m_name = registry.get<engine::NameComponent>(m_entity).name;
      m_id = registry.get<engine::IdentityComponent>(m_entity).id;
      //TODO: store entity snapshot
    } 

    void Execute() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().Get().registry;
      registry.destroy(m_entity);
    }

    void Undo() override
    {
      engine::Scene& scene = engine::SceneManager::GetActiveScene().Get();
      entt::entity entity = scene.CreateEntity<engine::TransformComponent, engine::WorldTransformComponent>(m_name);
      scene.registry.get<engine::IdentityComponent>(entity).id = m_id;
    }

  private:
    std::string m_name;
    engine::Uuid m_id;
    entt::entity m_entity;
  };
}