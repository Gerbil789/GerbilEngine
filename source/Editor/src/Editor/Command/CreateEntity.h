#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Components.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include <string_view>

namespace editor
{
  template<typename... Components>
  class CreateEntityCommand : public ICommand
  {
  public:
    CreateEntityCommand(std::string_view name = "Empty Entity", entt::entity parent = entt::null) : m_Name(name), m_Parent(parent) {}

    void Execute() override
    {
      engine::Scene scene = engine::SceneManager::GetActiveScene();
      entt::registry& registry = scene.GetRegistry();

      m_Entity = scene.CreateEntity<Components...>(m_Name);

      if (m_Parent != entt::null)
      {
        auto& hc = registry.get<engine::HierarchyComponent>(m_Entity);
        hc.parent = m_Parent;
        registry.get<engine::HierarchyComponent>(m_Parent).children.push_back(m_Entity);
      }
      else
      {
        scene.InsertRootEntity(m_Entity, scene.GetRootEntities().size()); //TODO: this is bad
      }

      //engine::Uuid id = registry.get<engine::IdentityComponent>(m_Entity).id;
      // SelectionManager::Assets.Select(id);

      FocusEntityEvent e{ m_Entity };
      engine::EventBus::Publish(e);
    }

    void Undo() override
    {
      if (m_Entity == entt::null) return;

      entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();
      registry.destroy(m_Entity);
    }

  private:
    std::string m_Name;
    entt::entity m_Parent;
    entt::entity m_Entity;
  };
}