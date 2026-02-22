#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/EditorSelection.h"

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
				EditorSelection::Entities().Clear();
        m_Entity.Destroy();
      }
    }

    void Undo() override
    {
      m_Entity = Engine::SceneManager::GetActiveScene()->CreateEntity(m_Name);
			EditorSelection::Entities().Select(m_Entity);
    }

  private:
    std::string m_Name;
    Engine::Entity m_Entity;



  };
}