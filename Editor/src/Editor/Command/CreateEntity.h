#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include "Engine/Asset/AssetManager.h"

namespace Editor
{
  class CreateEntityCommand : public ICommand 
  {
  public:
    CreateEntityCommand(Engine::Uuid sceneId, const std::string& name = "Empty Entity") : m_SceneId(sceneId), m_Name(name) {}

    void Execute() override 
    {
			Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(m_SceneId);
			m_Entity = scene.CreateEntity(m_Name);
			Engine::Uuid id = m_Entity.GetComponent<Engine::IdentityComponent>().id;

			SelectionManager::Assets.Select(id);
      FocusEntityEvent e{ id };
			Engine::EventBus::Publish(e);
    }

    void Undo() override 
    {
      if (!m_Entity) return;

      FocusEntityEvent e {0};
      Engine::EventBus::Publish(e);
      m_Entity.Destroy();
    }

  private:
		Engine::Uuid m_SceneId;
    std::string m_Name;
    Engine::Entity m_Entity;
  };
}