#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Core/SelectionManager.h"

namespace Editor
{
  class CreateEntityCommand : public ICommand 
  {
  public:
    CreateEntityCommand(const std::string& name = "Empty Entity") : m_Name(name) {}

    void Execute() override 
    {
      m_Entity = Engine::SceneManager::GetActiveScene().CreateEntity(m_Name);
			SelectionManager::Select(SelectionType::Entity, m_Entity.GetUUID());
    }

    void Undo() override 
    {
      if (m_Entity)
      {
				SelectionManager::Clear(SelectionType::Entity);
        m_Entity.Destroy();
      }
    }

  private:
    std::string m_Name;
    Engine::Entity m_Entity;
  };
}