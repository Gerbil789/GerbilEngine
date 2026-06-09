#pragma once

#include "Engine/Core/UUID.h"

namespace Editor
{
  enum class SelectionContext { Entity, Asset };

  class SelectionGroup
  {
  public:
    SelectionGroup(SelectionContext context) : m_Context(context) {}

    void Select(Engine::Uuid id, bool additive = false);
    void Toggle(Engine::Uuid id);
    void Clear();
    bool IsSelected(Engine::Uuid id) const;
    Engine::Uuid GetPrimary() const;
    const std::vector<Engine::Uuid>& GetAll() const;

  private:
    SelectionContext m_Context;
    std::vector<Engine::Uuid> m_Selection;
  };


  class SelectionManager
  {
  public:
    inline static SelectionGroup Entities{ SelectionContext::Entity };
    inline static SelectionGroup Assets{ SelectionContext::Asset };
  };
}