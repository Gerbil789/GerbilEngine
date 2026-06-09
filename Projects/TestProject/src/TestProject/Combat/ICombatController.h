#pragma once
#include "Engine/Core/UUID.h"

class ICombatController 
{
public:
  virtual ~ICombatController() = default;
  virtual void BeginTurn(Engine::Uuid activeUnitId) = 0;
  virtual void Update() = 0;
  virtual void CancelTurn() = 0;
};