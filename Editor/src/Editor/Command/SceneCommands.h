#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
  class CreateEntityCommand : public ICommand 
  {
  public:
    CreateEntityCommand(const std::string& name = "Empty Entity") : m_Name(name) {}

    void Execute() override 
    {
			Engine::Scene* scene = Engine::SceneManager::GetActiveScene();
      m_Entity = scene->CreateEntity(m_Name);
    }

    void Undo() override 
    {
      if (m_Entity)
      {
        m_Entity.Destroy();
        //Engine::Scene* scene = Engine::SceneManager::GetActiveScene();
        //scene->DestroyEntity(m_Entity);
				//TODO: deselct entity if selected
      }
    }

    Engine::Entity GetEntity() const { return m_Entity; }

  private:
    std::string m_Name;
    Engine::Entity m_Entity;

  };
}