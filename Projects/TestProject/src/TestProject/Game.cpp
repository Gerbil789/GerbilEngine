#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"

#include "TestProject/PlayerController.h"
#include "TestProject/RandomMovement.h"

using namespace Engine;

extern "C"
{
  GAME_API void RegisterScripts(Engine::ScriptRegistry& registry)
  {
    std::vector<ScriptField> fields
    {
        { "MoveSpeed", ScriptFieldType::Float, offsetof(PlayerController, MoveSpeed) },
        { "RotateSpeed", ScriptFieldType::Float, offsetof(PlayerController, RotateSpeed) },
        { "Rotate", ScriptFieldType::Bool, offsetof(PlayerController, Rotate) },
				{ "TestTexture", ScriptFieldType::Texture, offsetof(PlayerController, TestTexture) }
    };
    
    registry.Register<PlayerController>(fields);
    registry.Register<RandomMovement>({});
  }
}