#pragma once

#include "Engine/Core/UUID.h"
#include <string>
#include <cstdint>

enum class MovementType { Walk, Fly, Stationary };
enum class AttackType { Melee, Ranged };
enum class Team { Attacker, Defender };

struct Unit 
{
  Engine::Uuid id;
  std::string name;

  MovementType movementType = MovementType::Walk;
  AttackType attackType = AttackType::Melee;
  Team team = Team::Attacker;

  int32_t maxHealth = 10;
  int32_t currentHealth = 10;

  int32_t attack = 5;
  int32_t minDamage = 1;
  int32_t maxDamage = 3;

  int32_t initiative = 10;
  int32_t speed = 6;
};