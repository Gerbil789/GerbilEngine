#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Script/ScriptDescriptor.h"

using RegisterScriptFn = void(*)(const Engine::ScriptDescriptor&);

struct GameContext
{
	Engine::Scene* CurrentScene = nullptr;
	RegisterScriptFn RegisterScript;
};