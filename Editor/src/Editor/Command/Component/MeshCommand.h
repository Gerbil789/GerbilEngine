#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
  class RemoveMeshComponentCommand : public ICommand
  {
  public:
    RemoveMeshComponentCommand(Engine::Entity e) : m_Entity(e) {}

    void Execute() override
    {
      m_Entity.RemoveComponent<Engine::MeshComponent>();
    }

    void Undo() override
    {
      m_Entity.AddComponent<Engine::MeshComponent>(m_Backup);
    }

  private:
    Engine::Entity m_Entity;
    Engine::MeshComponent m_Backup;
  };

  class ResetMeshComponentCommand : public ICommand
  {
  public:
    ResetMeshComponentCommand(Engine::Entity e) : m_Entity(e) {}

    void Execute() override
    {
			auto& component = m_Entity.GetComponent<Engine::MeshComponent>();
      m_Backup = component;
      component.mesh = nullptr;
      component.material = nullptr;
    }

    void Undo() override
    {
      auto& component = m_Entity.GetComponent<Engine::MeshComponent>();
      component = m_Backup;
    }

  private:
    Engine::Entity m_Entity;
    Engine::MeshComponent m_Backup;
  };

}

