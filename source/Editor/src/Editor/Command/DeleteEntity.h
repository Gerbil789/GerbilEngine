#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/Scene.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

namespace Editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(Engine::Uuid entityId) : m_EntityId(entityId)
    {
			Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());

			Engine::Entity entity = scene.GetEntity(m_EntityId);

      if (entity)
      {
        m_Name = entity.GetComponent<Engine::NameComponent>().name;
			}
    } 

    void Execute() override
    {
      Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());
			Engine::Entity entity = scene.GetEntity(m_EntityId);
      entity.Destroy();
    }

    void Undo() override
    {
      Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());
      Engine::Entity entity = scene.CreateEntity<Engine::TransformComponent, Engine::WorldTransformComponent>(m_Name);

      auto& idc = entity.GetComponent<Engine::IdentityComponent>();
			idc.id = m_EntityId;
    }

  private:
    std::string m_Name;
    Engine::Uuid m_EntityId;
  };
}