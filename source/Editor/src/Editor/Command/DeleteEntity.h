#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/Scene.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

namespace editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(entt::entity entity) : m_Entity(entity)
    {
      if (entity == entt::null) return;

      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();
      m_Name = registry.get<engine::NameComponent>(m_Entity).name;
      m_Id = registry.get<engine::IdentityComponent>(m_Entity).id;
      //TODO: store entity snapshot
    } 

    void Execute() override
    {
      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();
      registry.destroy(m_Entity);
    }

    void Undo() override
    {
      engine::Scene scene = engine::SceneManager::GetActiveScene();
      entt::registry& registry = scene.GetRegistry();

      entt::entity entity = scene.CreateEntity<engine::TransformComponent, engine::WorldTransformComponent>(m_Name);

      registry.get<engine::IdentityComponent>(entity).id = m_Id;
    }

  private:
    std::string m_Name;
    engine::Uuid m_Id;
    entt::entity m_Entity;
  };
}