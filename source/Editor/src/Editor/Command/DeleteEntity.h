#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Core/Scene.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

namespace editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(engine::Uuid entityId) : m_EntityId(entityId)
    {
			engine::SceneAsset& scene = engine::AssetManager::GetAsset<engine::SceneAsset>(engine::SceneManager::GetActiveScene());

			engine::Entity entity = scene.GetEntity(m_EntityId);

      if (entity)
      {
        m_Name = entity.GetComponent<engine::NameComponent>().name;
			}
    } 

    void Execute() override
    {
      engine::SceneAsset& scene = engine::AssetManager::GetAsset<engine::SceneAsset>(engine::SceneManager::GetActiveScene());
			engine::Entity entity = scene.GetEntity(m_EntityId);
      entity.Destroy();
    }

    void Undo() override
    {
      engine::SceneAsset& scene = engine::AssetManager::GetAsset<engine::SceneAsset>(engine::SceneManager::GetActiveScene());
      engine::Entity entity = scene.CreateEntity<engine::TransformComponent, engine::WorldTransformComponent>(m_Name);

      auto& idc = entity.GetComponent<engine::IdentityComponent>();
			idc.id = m_EntityId;
    }

  private:
    std::string m_Name;
    engine::Uuid m_EntityId;
  };
}