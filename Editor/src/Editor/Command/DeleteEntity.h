#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Core/SelectionManager.h"

namespace Editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(Engine::Scene* scene, entt::entity entity) : m_Scene(scene), m_Entity(entity)
    {
      if (entity != entt::null)
      {
        m_Name = m_Scene->GetRegistry().get<Engine::NameComponent>(entity).name;
        m_Entity = entity;
			}
    } 

    void Execute() override
    {
      if (m_Entity == entt::null) return;

      SelectionManager::Clear(SelectionType::Entity);
      m_Scene->GetRegistry().destroy(m_Entity);
    }

    void Undo() override
    {
      Engine::Scene& scene = *m_Scene;
      entt::registry& registry = scene.GetRegistry();

      m_Entity = scene.CreateEntity(m_Name);
			auto id = registry.get<Engine::IdentityComponent>(m_Entity).id;

			SelectionManager::Select(SelectionType::Entity, id);
    }

  private:
		Engine::Scene* m_Scene;
    std::string m_Name;
		entt::entity m_Entity = entt::null;
  };
}