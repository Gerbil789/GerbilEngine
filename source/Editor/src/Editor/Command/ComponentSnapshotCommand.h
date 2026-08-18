#pragma once

#include "ICommand.h"
#include "Engine/Core/Entity.h"
#include "Engine/Core/Components.h"

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
      m_Entity.AddTag<Engine::TransformDirty>();
    }

    void Undo() override
    {
			m_Entity.GetComponent<T>() = m_Before;
      m_Entity.AddTag<Engine::TransformDirty>();
    }

  private:
    Engine::Entity m_Entity;
    T m_Before, m_After;
  };
}