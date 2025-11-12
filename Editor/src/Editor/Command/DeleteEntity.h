#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/EditorContext.h"

namespace Editor
{
  class DeleteEntityCommand : public ICommand
  {
  public:
    DeleteEntityCommand(Engine::Entity entity) : m_Entity(entity), m_Name(entity.GetName()) {}

    void Execute() override
    {
      if (m_Entity)
      {
				EditorContext::ClearSelection();
        m_Entity.Destroy();
      }

    }

    void Undo() override
    {
      m_Entity = Engine::SceneManager::GetActiveScene()->CreateEntity(m_Name);
      EditorContext::SelectEntity(m_Entity);
    }

  private:
    std::string m_Name;
    Engine::Entity m_Entity;



  };
}