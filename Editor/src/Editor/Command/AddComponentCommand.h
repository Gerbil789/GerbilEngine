#pragma once

#include "ICommand.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
  template<typename T>
  class AddComponentCommand : public ICommand
  {
  public:
    AddComponentCommand(Engine::Entity entity, const T& initial = {}) : m_Entity(entity), m_Initial(initial) {}

    void Execute() override
    {
      if (!m_Entity.HasComponent<T>())
      {
        m_Entity.AddComponent<T>(m_Initial);
      }
    }

    void Undo() override
    {
      if (m_Entity.HasComponent<T>())
      {
        m_Entity.RemoveComponent<T>();
      }
    }

  private:
    Engine::Entity m_Entity;
    T m_Initial;
  };
}