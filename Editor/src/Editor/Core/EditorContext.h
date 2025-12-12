#pragma once

#include "Engine/Scene/Entity.h"

namespace Editor
{
  enum class SelectionType
  {
    None,
    Entity,
    Asset
  };

  struct Selection
  {
    SelectionType Type = SelectionType::None;
    Engine::Entity Entity = Engine::Entity::Null();
    Engine::UUID AssetID = 0;
  };

  class EditorContext
  {
  public:
    static void SelectEntity(Engine::Entity entity, bool additive = false)
    {
			LOG_TRACE("Selecting entity: {0}", (uint32_t)entity);

      if (!additive)
      {
        m_Selection.clear();
      }

      m_Selection.push_back({ SelectionType::Entity, entity, 0 });

			LOG_TRACE("Selected entities: {0}", m_Selection.size());
    }

   
    static void SelectAsset(Engine::UUID id)
    {
      m_Selection.clear();
      m_Selection.push_back({ SelectionType::Asset, Engine::Entity::Null(), id });
    }

    static void ClearSelection()
    {
      m_Selection.clear();
    }

    static const std::vector<Selection>& GetSelection() { return m_Selection; }

    static Selection GetActiveSelection()
    {
      return m_Selection.empty() ? Selection{} : m_Selection.front();
    }

  private:
    inline static std::vector<Selection> m_Selection;
  };
}