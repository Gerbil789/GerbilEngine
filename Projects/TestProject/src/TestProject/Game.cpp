#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"

#include "TestProject/PlayerController.h"
#include "TestProject/RandomMovement.h"
#include "TestProject/AudioPlayer.h"
#include "TestProject/GameManager.h"
#include "TestProject/Button.h"

extern "C"
{
  GAME_API void RegisterScripts(Engine::ScriptRegistry& registry)
  {
    registry.Register<PlayerController>();
    registry.Register<RandomMovement>();
		registry.Register<AudioPlayer>();
		registry.Register<GameManager>();
    registry.Register<Button>();
  }
}