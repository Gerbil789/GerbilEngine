#pragma once

#include "AssetSelection.h"
#include "EntitySelection.h"

namespace Editor
{
  class EditorContext
  {
  public:
    static EntitySelection& Entities() { return m_EntitySelection; }
    static AssetSelection& Assets() { return m_AssetSelection; }

    static void Select(Engine::Entity entity, bool additive = false)
    {
			m_AssetSelection.Clear();
      m_EntitySelection.Select(entity, additive);
		}

    static void Select(Engine::UUID assetID)
		{
      m_EntitySelection.Clear();
			m_AssetSelection.Select(assetID);
		}

    static void ClearAll()
    {
      m_EntitySelection.Clear();
      m_AssetSelection.Clear();
    }

  private:
    inline static EntitySelection m_EntitySelection;
    inline static AssetSelection  m_AssetSelection;
  };
}