#include "Engine/Script/ScriptRegistry.h"
#include "TestProject/PlayerController.h"
#include <print>

using namespace Engine;

extern "C"
{
  GAME_API void RegisterScripts(Engine::ScriptRegistry& registry)
  {
    std::println("RegisterScripts function called from Game DLL");

    std::vector<ScriptField> fields
    {
        { "Speed", ScriptFieldType::Float, offsetof(PlayerController, Speed) },
        { "GodMode", ScriptFieldType::Bool, offsetof(PlayerController, GodMode) }
    };

    registry.Register<PlayerController>("PlayerController", fields);
  }

  //GAME_API void OnLoad(GameContext*)
  //{
		//std::println("OnLoad function called from Game DLL");
  //}

  //GAME_API void OnUnload()
  //{
		//std::println("OnUnload function called from Game DLL");
  //}

  //GAME_API void OnStart()
  //{
		//std::println("OnStart function called from Game DLL");
  //}

  //GAME_API void OnUpdate([[maybe_unused]] float delta)
  //{
		////std::println("OnUpdate function called from Game DLL with delta: {}", delta);
  //}
}