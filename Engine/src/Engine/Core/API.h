#pragma once

#include "Engine/Core/GameContext.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

//using namespace Engine;

#ifdef ENGINE_PLATFORM_WINDOWS
    #ifdef GAME_BUILD_DLL
        #define GAME_API  __declspec(dllexport)
    #else
        #define GAME_API  __declspec(dllimport)
    #endif
#else
    #define GAME_API
#endif



extern "C"
{
  GAME_API void Game_OnLoad(GameContext* context);
  GAME_API void Game_OnUnload();
  GAME_API void Game_Start();
  GAME_API void Game_Update(float delta);
}