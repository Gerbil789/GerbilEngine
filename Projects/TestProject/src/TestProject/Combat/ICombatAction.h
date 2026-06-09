#pragma once

#include "Engine/Core/UUID.h"

class Grid;

//enum class TargetRestriction { Any, AllyOnly, EnemyOnly };


class ICombatAction 
{
public:
  virtual ~ICombatAction() = default;
  virtual void BeginTargeting(Engine::Uuid casterId) = 0;
  virtual void UpdateTargeting(Grid* grid, int mouseX, int mouseY) = 0;
  virtual void CancelTargeting() = 0;
  virtual void Execute(Engine::Uuid casterId, Grid* grid, int targetX, int targetY) = 0;
};