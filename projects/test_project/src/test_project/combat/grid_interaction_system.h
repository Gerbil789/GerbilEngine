#pragma once

#include "../math/ray_intersects_plane.h" //TODO: fix paths, dont ever use "../"
#include "game_context.h"
#include "engine/core/components.h"
#include "engine/system/camera_system.h"
#include "engine/core/log.h"
#include "engine/core/input.h"
#include "engine/core/components.h"

struct GridInteractionsystem
{
  GridInteractionsystem() = default;

  // void OnMouseButtonReleased(engine::MouseButtonReleasedEvent& e)
  // {
  //   if (e.button == engine::Mouse::ButtonLeft)
  //   {
  //     if (GameContext::hoveredTile)
  //     {
  //       LOG_TRACE("Hovered tile: ({}, {})", GameContext::hoveredTile->x, GameContext::hoveredTile->y);
  //     }
  //     else
  //     {
	// 			LOG_TRACE("No tile hovered");
  //     }
  //   }
  // }

  void Update()
  {
    // glm::vec2 mousePos = engine::input::GetMousePosition();

		// mousePos.x = mousePos.x - engine::viewportState.positionX;
		// mousePos.y = mousePos.y - engine::viewportState.positionY;
    // //LOG_TRACE("Mouse Position: ({}, {})", mousePos.x, mousePos.y);

    // bool inside = mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < engine::viewportState.width && mousePos.y < engine::viewportState.height;

    // if(!inside)
    // {
    //   GameContext::hoveredTile = nullptr;
    //   GameContext::mousePosition = { -1.0f, -1.0f };
    //   return;
		// }

    // glm::vec3 rayOrigin, rayDir;



    //camera.ScreenToWorldRay(mousePos.x, mousePos.y, rayOrigin, rayDir);

    //entt::entity cameraEntity = scene


    //engine::Camerasystem::ScreenToWorldRay(mousePos.x, mousePos.y, rayOrigin, rayDir);


    // glm::vec3 hitPoint;
    // glm::vec3 floorNormal = { 0.0f, 1.0f, 0.0f };
    // glm::vec3 floorCenter = { 0.0f, 0.0f, 0.0f };

    // const glm::vec3 gridPosition = { 0,0,0 };
    // const float tileSize = 4.0f;

    // if (RayIntersectsPlane(rayOrigin, rayDir, floorNormal, floorCenter, hitPoint))
    // {
    //   glm::vec3 local = hitPoint - gridPosition;

    //   local.x += GameContext::grid.GetWidth() * 0.5f * tileSize;
    //   local.z += GameContext::grid.GetHeight() * 0.5f * tileSize;

    //   int x = static_cast<int>(std::floor(local.x / tileSize));
    //   int y = static_cast<int>(std::floor(local.z / tileSize));

		// 	GameContext::mousePosition = { x, y };

		// 	GameContext::hoveredTile = GameContext::grid.GetTile(x, y);
    // }
    // else
    // {
    //   GameContext::hoveredTile = nullptr;
    //   GameContext::mousePosition = { -1.0f, -1.0f };

    // }
  }


  //glm::vec3 GridToWorld(int x, int y)
  //{
  //  float worldX = (x + 0.5f) * tileSize - (width * tileSize * 0.5f);
  //  float worldZ = (y + 0.5f) * tileSize - (height * tileSize * 0.5f);

  //  return position + glm::vec3(worldX, 0.0f, worldZ);
  //}
};