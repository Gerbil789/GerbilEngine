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
    DeleteEntityCommand(Engine::Entity entity) : m_Entity(entity)
    {
      if (entity.IsValid())
      {
        m_Name = m_Entity.GetComponent<Engine::NameComponent>().name;
        m_Entity = entity;
        m_Scene = m_Entity.GetScene();
			}
    } 

    void Execute() override
    {
      if (!m_Entity.IsValid()) return;

      FocusEntityEvent e{0};
			Engine::EventBus::Publish(e);
      m_Entity.Destroy();
    }

    void Undo() override
    {
      m_Entity = m_Scene->CreateEntity(m_Name);
			Engine::Uuid id = m_Entity.GetComponent<Engine::IdentityComponent>().id;

      FocusEntityEvent e{ id };
      Engine::EventBus::Publish(e);
    }

  private:
		Engine::Scene* m_Scene;
    std::string m_Name;
    Engine::Entity m_Entity;
  };
}