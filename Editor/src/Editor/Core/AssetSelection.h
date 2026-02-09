#pragma once

#include "Engine/Core/UUID.h"

namespace Editor
{
  class AssetSelection
  {
  public:
    void Clear();
    void Select(Engine::Uuid id);
    bool IsSelected(Engine::Uuid id) const;

		bool Empty() const { return m_Selected.empty(); }
    Engine::Uuid GetPrimary() const;

  private:
    std::vector<Engine::Uuid> m_Selected;
  };
}