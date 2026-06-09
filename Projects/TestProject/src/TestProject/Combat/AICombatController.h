#pragma once

#include "ICombatController.h"
#include "CombatManager.h"

class AIController : public ICombatController 
{
public:
  AIController(CombatManager* manager, Grid* grid) : m_combatManager(manager), m_grid(grid) {}

  void BeginTurn(Engine::Uuid activeUnitId) override 
  {
    m_activeUnitId = activeUnitId;
    m_isThinking = true;
  }

  void Update() override 
  {
    if (m_isThinking) {
      // Run A* pathfinding, evaluate best moves, execute actions
      m_isThinking = false;
    }
  }

  void CancelTurn() override 
  {
    m_isThinking = false;
  }

private:
  CombatManager* m_combatManager;
  Grid* m_grid;
  Engine::Uuid m_activeUnitId;
  bool m_isThinking = false;
};