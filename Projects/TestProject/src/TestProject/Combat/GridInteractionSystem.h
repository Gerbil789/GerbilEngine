#pragma once

#include "../Math/RayIntersectsPlane.h" //TODO: fix paths, dont ever use "../"
#include "GameContext.h"
#include "Engine/Scene/Components.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Core/State.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Input.h"
#include <entt.hpp>


#include "Engine/Event/EventBus.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Scene/Components.h"

class GridInteractionSystem
{
public:
  GridInteractionSystem() = default;

  void Initialize()
  {
    Engine::EventBus::Get().Subscribe<Engine::MouseButtonReleasedEvent>([this](auto e) {OnMouseButtonReleased(e); });
	}

  void OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e)
  {
    if (e.button == Engine::Mouse::ButtonLeft)
    {
      if (GameContext::hoveredTile)
      {
        LOG_TRACE("Hovered tile: ({}, {})", GameContext::hoveredTile->x, GameContext::hoveredTile->y);
      }
      else
      {
				LOG_TRACE("No tile hovered");
      }
    }
  }

  void Update(const Engine::Camera& camera)
  {
    glm::vec2 mousePos = Engine::Input::GetMousePosition();

		mousePos.x = mousePos.x - Engine::viewportState.positionX;
		mousePos.y = mousePos.y - Engine::viewportState.positionY;
    //LOG_TRACE("Mouse Position: ({}, {})", mousePos.x, mousePos.y);

    bool inside = mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < Engine::viewportState.width && mousePos.y < Engine::viewportState.height;

    if(!inside)
    {
      GameContext::hoveredTile = std::nullopt;
      return;
		}

    glm::vec3 rayOrigin, rayDir;
    camera.ScreenToWorldRay(mousePos.x, mousePos.y, rayOrigin, rayDir);

    glm::vec3 hitPoint;
    glm::vec3 floorNormal = { 0.0f, 1.0f, 0.0f };
    glm::vec3 floorCenter = { 0.0f, 0.0f, 0.0f };

    const glm::vec3 gridPosition = { 0,0,0 };
    const float tileSize = 4.0f;

    if (RayIntersectsPlane(rayOrigin, rayDir, floorNormal, floorCenter, hitPoint))
    {
      glm::vec3 local = hitPoint - gridPosition;

      local.x += GameContext::grid.GetWidth() * 0.5f * tileSize;
      local.z += GameContext::grid.GetHeight() * 0.5f * tileSize;

      int x = static_cast<int>(std::floor(local.x / tileSize));
      int y = static_cast<int>(std::floor(local.z / tileSize));

			GameContext::mousePosition = { x, y };

			GameContext::hoveredTile = GameContext::grid.GetTile(x, y);
    }
    else
    {
      GameContext::hoveredTile = std::nullopt;
      GameContext::mousePosition = { -1.0f, -1.0f };

    }
  }


  //glm::vec3 GridToWorld(int x, int y)
  //{
  //  float worldX = (x + 0.5f) * tileSize - (width * tileSize * 0.5f);
  //  float worldZ = (y + 0.5f) * tileSize - (height * tileSize * 0.5f);

  //  return position + glm::vec3(worldX, 0.0f, worldZ);
  //}
};