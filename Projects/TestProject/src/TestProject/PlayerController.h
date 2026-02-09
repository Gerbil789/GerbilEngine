#pragma once

#include "Engine/Script/Script.h"

struct PlayerController : public Engine::Script
{
  float Speed = 10.0f;
	bool GodMode = false;

	void OnCreate() override;
	void OnUpdate(float delta) override;
};