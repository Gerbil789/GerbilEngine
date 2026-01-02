#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/EditorContext.h"

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
				EditorContext::Entities().Clear();
        m_Entity.Destroy();
      }
    }

    void Undo() override
    {
      m_Entity = Engine::SceneManager::GetActiveScene()->CreateEntity(m_Name);
			EditorContext::Entities().Select(m_Entity);
    }

  private:
    std::string m_Name;
    Engine::Entity m_Entity;



  };
}