#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include "Engine/Asset/AssetManager.h"
#include <string_view>

namespace Editor
{
  template<typename... Components>
  class CreateEntityCommand : public ICommand
  {
  public:
    CreateEntityCommand(std::string_view name = "Empty Entity", entt::entity parent = entt::null) : m_Name(name), m_Parent(parent) {}

    void Execute() override
    {
      Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());

      // Forward the component pack directly into scene.CreateEntity
      m_Entity = scene.CreateEntity<Components...>(m_Name);

      if (m_Parent != entt::null)
      {
        auto& hc = m_Entity.GetComponent<Engine::HierarchyComponent>();
        hc.parent = m_Parent;
        scene.GetRegistry().get<Engine::HierarchyComponent>(m_Parent).children.push_back(m_Entity.GetHandle());
      }
      else
      {
        scene.InsertRootEntity(m_Entity.GetHandle(), scene.GetRootEntities().size());
      }

      Engine::Uuid id = m_Entity.GetComponent<Engine::IdentityComponent>().id;
      SelectionManager::Assets.Select(id);
      FocusEntityEvent e{ id };
      Engine::EventBus::Publish(e);
    }

    void Undo() override
    {
      if (!m_Entity) return;

      FocusEntityEvent e{ 0 };
      Engine::EventBus::Publish(e);
      m_Entity.Destroy();
    }

  private:
    entt::entity m_Parent;
    std::string m_Name; // Stores the name safely for the command lifetime
    Engine::Entity m_Entity;
  };
}