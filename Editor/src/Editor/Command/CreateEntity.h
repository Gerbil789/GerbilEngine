#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

namespace Editor
{
  class CreateEntityCommand : public ICommand 
  {
  public:
    CreateEntityCommand(Engine::Scene* scene, const std::string& name = "Empty Entity") : m_Scene(scene), m_Name(name) {}

    void Execute() override 
    {
			m_Entity = m_Scene->CreateEntity(m_Name);
			auto id = m_Scene->GetRegistry().get<Engine::IdentityComponent>(m_Entity).id;

			//SelectionManager::Select(SelectionType::Entity, id);
      FocusEntityEvent e{ id };
			Engine::EventBus::Get().Publish(e);
    }

    void Undo() override 
    {
      if (m_Entity == entt::null) return;

      FocusEntityEvent e {0};
      Engine::EventBus::Get().Publish(e);
      m_Scene->GetRegistry().destroy(m_Entity);
    }

  private:
		Engine::Scene* m_Scene;
    std::string m_Name;
		entt::entity m_Entity = entt::null;
  };
}