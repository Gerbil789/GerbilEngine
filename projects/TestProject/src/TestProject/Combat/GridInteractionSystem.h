#pragma once

#include "../Math/RayIntersectsPlane.h" //TODO: fix paths, dont ever use "../"
#include "GameContext.h"
#include "Engine/Core/Components.h"
#include "Engine/System/CameraSystem.h"
#include "Engine/Core/State.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Input.h"

#include "Engine/Event/EventBus.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Core/Components.h"

class GridInteractionSystem
{
public:
  GridInteractionSystem() = default;

  void Initialize()
  {
    engine::EventBus::Subscribe<engine::MouseButtonReleasedEvent>([this](auto e) {OnMouseButtonReleased(e); return false; });
	}

  void OnMouseButtonReleased(engine::MouseButtonReleasedEvent& e)
  {
    if (e.button == engine::Mouse::ButtonLeft)
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

  void Update()
  {
    glm::vec2 mousePos = engine::Input::GetMousePosition();

		mousePos.x = mousePos.x - engine::viewportState.positionX;
		mousePos.y = mousePos.y - engine::viewportState.positionY;
    //LOG_TRACE("Mouse Position: ({}, {})", mousePos.x, mousePos.y);

    bool inside = mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < engine::viewportState.width && mousePos.y < engine::viewportState.height;

    if(!inside)
    {
      GameContext::hoveredTile = nullptr;
      GameContext::mousePosition = { -1.0f, -1.0f };
      return;
		}

    glm::vec3 rayOrigin, rayDir;



    //camera.ScreenToWorldRay(mousePos.x, mousePos.y, rayOrigin, rayDir);

    //entt::entity cameraEntity = scene


    //Engine::CameraSystem::ScreenToWorldRay(mousePos.x, mousePos.y, rayOrigin, rayDir);


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
      GameContext::hoveredTile = nullptr;
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