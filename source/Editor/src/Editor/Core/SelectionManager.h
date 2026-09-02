#pragma once

#include "Engine/Core/UUID.h"

namespace editor
{
  enum class SelectionContext { Entity, Asset };

  class SelectionGroup
  {
  public:
    SelectionGroup(SelectionContext context) : m_Context(context) {}

    void Select(engine::Uuid id, bool additive = false);
    void Toggle(engine::Uuid id);
    void Clear();
    bool IsSelected(engine::Uuid id) const;
    engine::Uuid GetPrimary() const;
    const std::vector<engine::Uuid>& GetAll() const;

  private:
    SelectionContext m_Context;
    std::vector<engine::Uuid> m_Selection;
  };


  class SelectionManager
  {
  public:
		static void Initialize();

    inline static SelectionGroup Entities{ SelectionContext::Entity };
    inline static SelectionGroup Assets{ SelectionContext::Asset };
  };
}