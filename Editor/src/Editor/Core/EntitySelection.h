#pragma once

#include "Engine/Scene/Entity.h"

namespace Editor
{
  class EntitySelection
  {
  public:
    void Clear();
    void Select(Engine::Entity entity, bool additive = false);
    bool IsSelected(Engine::Entity entity) const;

		bool Empty() const { return m_Selected.empty(); }
    Engine::Entity GetPrimary() const;
    const std::vector<Engine::Entity>& GetAll() const;

  private:
    std::vector<Engine::Entity> m_Selected;
  };
}