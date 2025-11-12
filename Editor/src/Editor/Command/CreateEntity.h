#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/EditorContext.h"

namespace Editor
{
  class CreateEntityCommand : public ICommand 
  {
  public:
    CreateEntityCommand(const std::string& name = "Empty Entity") : m_Name(name) {}

    void Execute() override 
    {
      m_Entity = Engine::SceneManager::GetActiveScene()->CreateEntity(m_Name);
			EditorContext::SelectEntity(m_Entity);
    }

    void Undo() override 
    {
      if (m_Entity)
      {
				EditorContext::ClearSelection();
        m_Entity.Destroy();
      }
    }

  private:
    std::string m_Name;
    Engine::Entity m_Entity;

  };
}