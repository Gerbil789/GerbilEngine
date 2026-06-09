#pragma once

#include "ICombatController.h"
#include "CombatManager.h"
#include "Grid.h"

class PlayerController : public ICombatController {
public:
  PlayerController(CombatManager* manager, Grid* grid) : m_combatManager(manager), m_grid(grid) {}

  void BeginTurn(Engine::Uuid activeUnitId) override 
  {
    m_activeUnitId = activeUnitId;
    m_currentState = TurnState::ChoosingAction;
    // Unlock the UI buttons for the player
  }

  void Update() override 
  {
    // Run the targeting state machine we discussed earlier
    // Check mouse inputs, update BG3 style visual previews, etc.
  }

  void CancelTurn() override 
  {
    // Clean up targeting visuals instantly
  }

private:
  CombatManager* m_combatManager;
  Grid* m_grid;
  Engine::Uuid m_activeUnitId;
	TurnState m_currentState = TurnState::Idle;
};