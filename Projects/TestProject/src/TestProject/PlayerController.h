#pragma once

namespace Engine
{
  class Entity;
}


struct PlayerController
{
  float Speed = 5.0f;
};

void Player_OnCreate(PlayerController&, Engine::Entity);
void Player_OnUpdate(PlayerController&, Engine::Entity, float);