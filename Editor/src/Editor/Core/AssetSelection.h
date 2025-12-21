#pragma once

#include "Engine/Core/UUID.h"

namespace Editor
{
  class AssetSelection
  {
  public:
    void Clear();
    void Select(Engine::UUID id);
    bool IsSelected(Engine::UUID id) const;

		bool Empty() const { return m_Selected.empty(); }
    Engine::UUID GetPrimary() const;

  private:
    std::vector<Engine::UUID> m_Selected;
  };
}