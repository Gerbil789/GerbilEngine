#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/SelectionManager.h"

namespace Editor
{
  class CreateEntityCommand : public ICommand 
  {
  public:
    CreateEntityCommand(const std::string& name = "Empty Entity") : m_Name(name) {}

    void Execute() override 
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      m_Entity = scene.CreateEntity(m_Name);
			auto id = registry.get<Engine::IdentityComponent>(m_Entity).id;

			SelectionManager::Select(SelectionType::Entity, id);
    }

    void Undo() override 
    {
      if (m_Entity == entt::null) return;

      SelectionManager::Clear(SelectionType::Entity);

      Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
      entt::registry& registry = scene.GetRegistry();
			registry.destroy(m_Entity);
    }

  private:
    std::string m_Name;
    entt::entity m_Entity;
  };
}