#pragma once

#include "Editor/Core/EditorSettings.h"
#include "Engine/Asset/AssetType.h"
#include "Engine/Core/Components.h"
#include <entt/entity/entity.hpp>
#include <vector>
#include <algorithm>

namespace editor
{
  enum class EditorMode { Edit, Play };
  enum class PlayMode { PIE, Windowed, Fullscreen };
}

namespace editor::context
{
  inline EditorMode editorMode = EditorMode::Edit;
  inline PlayMode playMode = PlayMode::PIE;

  inline engine::component::CameraComponent camera;
  inline engine::component::Transform cameraTransform;

  inline EditorSettings settings;

  struct Asset 
  {
    engine::Uuid id = 0;
    engine::AssetType type = engine::AssetType::Unknown;
  };
  inline Asset selectedAsset;


  struct SelectionState
  {
    std::vector<entt::entity> entities;
    entt::entity primary = entt::null;

		bool IsSelected(entt::entity e) const
    {
      return std::find(entities.begin(), entities.end(), e) != entities.end();
    }
  };

  inline SelectionState currentSelection;
  inline SelectionState pendingSelection;

  inline void UpdateSelection()
  {
    currentSelection = pendingSelection;
  }

  inline void SelectEntity(entt::entity entity, bool additive = false)
  {
    if (entity == entt::null)
    {
      pendingSelection.entities.clear();
      pendingSelection.primary = entt::null;
      return;
    }

    if (!additive)
    {
      pendingSelection.entities = { entity };
      pendingSelection.primary = entity;
    }
    else
    {
      auto& ents = pendingSelection.entities;
      auto it = std::find(ents.begin(), ents.end(), entity);
      if (it != ents.end())
      {
        pendingSelection.primary = entity;
      }
      else
      {
        ents.push_back(entity);
        pendingSelection.primary = entity;
      }
    }
  }

  inline void DeselectEntity(entt::entity entity)
  {
    auto& ents = pendingSelection.entities;
    std::erase(ents, entity);

    if (pendingSelection.primary == entity)
    {
      pendingSelection.primary = ents.empty() ? entt::null : ents.back();
    }
  }
}