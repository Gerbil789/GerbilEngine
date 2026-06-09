#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"

#include "TestProject/RandomTestStuff/FPSPlayerController.h"
#include "TestProject/RandomTestStuff/RandomMovement.h"
#include "TestProject/RandomTestStuff/AudioPlayer.h"
#include "TestProject/RandomTestStuff/GameManager.h"
#include "TestProject/RandomTestStuff/Button.h"


#include "TestProject/Combat/BattleControllerScript.h"

extern "C"
{
  GAME_API void RegisterScripts(Engine::ScriptRegistry& registry)
  {
    // random test scripts
    registry.Register<FPSPlayerController>();
    registry.Register<RandomMovement>();
		registry.Register<AudioPlayer>();
		registry.Register<GameManager>();
    registry.Register<Button>();

		// combat game scripts
		registry.Register<BattleControllerScript>();
  }
}