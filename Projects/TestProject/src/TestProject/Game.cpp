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
  __declspec(dllexport) void RegisterScripts()
  {
    // random test scripts
    Engine::ScriptRegistry::Register<FPSPlayerController>("FPSPlayerController");
    Engine::ScriptRegistry::Register<RandomMovement>("RandomMovement");
		Engine::ScriptRegistry::Register<AudioPlayer>("AudioPlayer");
		Engine::ScriptRegistry::Register<GameManager>("GameManager");
    Engine::ScriptRegistry::Register<Button>("Button");

		// combat game scripts
    Engine::ScriptRegistry::Register<BattleControllerScript>("BattleControllerScript");
  }
}