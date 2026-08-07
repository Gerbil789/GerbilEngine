#pragma once

#include "ICommand.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

namespace Editor
{
  template<typename T>
  class ComponentSnapshotCommand : public ICommand
  {
  public:
    ComponentSnapshotCommand(Engine::Entity e, T before, T after) : m_Entity(e), m_Before(before), m_After(after) {}

    void Execute() override
    {
			m_Entity.GetComponent<T>() = m_After;
      m_Entity.SetDirty<Engine::DirtyTag>();
    }

    void Undo() override
    {
			m_Entity.GetComponent<T>() = m_Before;
      m_Entity.SetDirty<Engine::DirtyTag>();
    }

  private:
    Engine::Entity m_Entity;
    T m_Before, m_After;
  };
}