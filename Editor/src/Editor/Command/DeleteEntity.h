#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/SelectionManager.h"

namespace Editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(Engine::Entity entity) : m_Name(entity.GetName()), m_Entity(entity) {}

    void Execute() override
    {
      if (m_Entity)
      {
				SelectionManager::Clear(SelectionType::Entity);
        m_Entity.Destroy();
      }
    }

    void Undo() override
    {
      m_Entity = Engine::SceneManager::GetActiveScene().CreateEntity(m_Name);
			SelectionManager::Select(SelectionType::Entity, m_Entity.GetUUID());
    }

  private:
    std::string m_Name;
    Engine::Entity m_Entity;



  };
}