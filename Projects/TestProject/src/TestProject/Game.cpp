#include "enginepch.h"
#include "Engine/Core/API.h"
#include "TestProject/PlayerController.h"
#include <print>

using namespace Engine;

extern "C"
{
  GAME_API void Game_OnLoad(GameContext* context)
  {
		std::println("Game_OnLoad function called from Game DLL");

    ScriptDescriptor desc{};
    desc.Name = "PlayerController";
    desc.Size = sizeof(PlayerController);

    desc.OnCreate = [](void* instance, Entity e)
      {
        Player_OnCreate(*(PlayerController*)instance, e);
      };

    desc.OnUpdate = [](void* instance, Entity e, float dt)
      {
        Player_OnUpdate(*(PlayerController*)instance, e, dt);
      };

    desc.Fields = 
    {
        { "Speed", ScriptFieldType::Float, offsetof(PlayerController, Speed) }
    };

    context->RegisterScript(desc);
  }

  GAME_API void Game_OnUnload()
  {
		std::println("Game_OnUnload function called from Game DLL");
  }

  GAME_API void Game_Start()
  {
		std::println("Game_Start function called from Game DLL");
  }

  GAME_API void Game_Update(float delta)
  {



  }
}