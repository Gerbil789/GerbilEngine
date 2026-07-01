#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

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

      FocusEntityEvent e{0};
			Engine::EventBus::Publish(e);
      m_Scene->GetRegistry().destroy(m_Entity);
    }

    void Undo() override
    {
      Engine::Scene& scene = *m_Scene;
      entt::registry& registry = scene.GetRegistry();

      m_Entity = scene.CreateEntity(m_Name);
			Engine::Uuid id = registry.get<Engine::IdentityComponent>(m_Entity).id;

      FocusEntityEvent e{ id };
      Engine::EventBus::Publish(e);
    }

  private:
		Engine::Scene* m_Scene;
    std::string m_Name;
		entt::entity m_Entity = entt::null;
  };
}