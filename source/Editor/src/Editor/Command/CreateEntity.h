#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Components.h"
#include <string_view>

namespace editor
{
  template<typename... Components>
  class CreateEntityCommand : public ICommand
  {
  public:
    CreateEntityCommand(std::string_view name = "Empty Entity", entt::entity parent = entt::null) : m_name(name), m_parent(parent) {}

    void Execute() override
    {
      engine::Scene& scene = engine::SceneManager::GetActiveScene().Get();

      m_entity = scene.CreateEntity<Components...>(m_name);

      if (m_parent != entt::null)
      {
        auto& hc = scene.registry.get<engine::component::Hierarchy>(m_entity);
        hc.parent = m_parent;
        scene.registry.get<engine::component::Hierarchy>(m_parent).children.push_back(m_entity);
      }
      else
      {
        scene.InsertRootEntity(m_entity, scene.GetRootEntities().size()); //TODO: this is bad
      }
    }

    void Undo() override
    {
      if (m_entity == entt::null) return;

      entt::registry& registry = engine::SceneManager::GetActiveScene().Get().registry;
      registry.destroy(m_entity);
    }

  private:
    std::string m_name;
    entt::entity m_parent;
    entt::entity m_entity;
  };
}