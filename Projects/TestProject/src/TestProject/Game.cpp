#include "Engine/Core/API.h"
#include <iostream>

extern "C"
{
  GAME_API void Game_OnLoad(GameContext* context)
  {
		std::cout << "Game_OnLoad function called from Game DLL" << std::endl;

    if (context && context->CurrentScene)
    {
      std::cout << "Entities in the scene:" << std::endl;
      for (auto entity : context->CurrentScene->GetEntities())
      {
        std::string name = entity.GetName();
        std::cout << " - Entity ID: " << static_cast<uint32_t>(entity) << ", Name: " << name << std::endl;
      }
    }
    else
    {
      std::cout << "No valid scene found in GameContext." << std::endl;
		}
  }

  GAME_API void Game_OnUnload()
  {
    std::cout << "Game_OnUnload function called from Game DLL" << std::endl;
  }

  GAME_API void Game_Start()
  {
    std::cout << "Game_Start function called from Game DLL" << std::endl;
  }

  GAME_API void Game_Update(float delta)
  {
    std::cout << "Game_Update function called from Game DLL" << std::endl;
  }
}