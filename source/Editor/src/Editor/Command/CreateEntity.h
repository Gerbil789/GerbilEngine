#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Components.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include "Engine/Asset/AssetManager.h"
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
      engine::SceneAsset& scene = engine::AssetManager::GetAsset<engine::SceneAsset>(engine::SceneManager::GetActiveScene());

      // Forward the component pack directly into scene.CreateEntity
      m_Entity = scene.CreateEntity<Components...>(m_Name);

      if (m_Parent != entt::null)
      {
        auto& hc = m_Entity.GetComponent<engine::HierarchyComponent>();
        hc.parent = m_Parent;
        scene.GetRegistry().get<engine::HierarchyComponent>(m_Parent).children.push_back(m_Entity.GetHandle());
      }
      else
      {
        scene.InsertRootEntity(m_Entity.GetHandle(), scene.GetRootEntities().size());
      }

      engine::Uuid id = m_Entity.GetComponent<engine::IdentityComponent>().id;
      SelectionManager::Assets.Select(id);
      FocusEntityEvent e{ id };
      engine::EventBus::Publish(e);
    }

    void Undo() override
    {
      if (!m_Entity) return;
      m_Entity.Destroy();
    }

  private:
    std::string m_Name;
    entt::entity m_Parent;
    engine::Entity m_Entity;
  };
}