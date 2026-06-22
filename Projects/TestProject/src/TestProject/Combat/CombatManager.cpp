#include "CombatManager.h"
#include <algorithm>

CombatManager::CombatManager(ICombatController* attackerControl, ICombatController* defenderControl)
{
  m_controllers[Team::Attacker] = attackerControl;
  m_controllers[Team::Defender] = defenderControl;
}

void CombatManager::AddUnitToBattle(const Unit& unit)
{
  m_roster.push_back(unit);
}

void CombatManager::Update() 
{
  if (m_activeController) 
  {
    m_activeController->Update();
  }
}

void CombatManager::StartBattle() 
{
  m_roundNumber = 1;
  RollInitiative();
}

void CombatManager::RollInitiative() 
{
  m_turnQueue.clear();

  for (const auto& unit : m_roster) 
  {
    if (unit.stats.currentHealth > 0) 
    {
      m_turnQueue.push_back(unit.id);
    }
  }

  // Sort the queue based on the unit's initiative stat
  std::sort(m_turnQueue.begin(), m_turnQueue.end(), [this](Engine::Uuid a, Engine::Uuid b) 
    {
    Unit* unitA = GetUnit(a);
    Unit* unitB = GetUnit(b);

    // Safety check
    if (!unitA || !unitB) return false;

    // Highest initiative goes first. 
    // Note: You can add tie-breakers here (e.g., if initiative is equal, highest speed wins)
    return unitA->stats.initiative > unitB->stats.initiative;
    });

  m_currentTurnIndex = 0;
}

void CombatManager::NextTurn() 
{
  m_currentTurnIndex++;

  if (m_currentTurnIndex >= m_turnQueue.size()) 
  {
    m_roundNumber++;
    RollInitiative(); // Recalculate order for the next round
  }

  // Check if the newly active unit is dead (maybe it died from poison between turns)
  Unit* activeUnit = GetActiveUnit();
  if (activeUnit && activeUnit->stats.currentHealth <= 0) 
  {
    NextTurn(); // Recursively skip dead units
  }

  m_activeController = m_controllers[activeUnit->team];
  m_activeController->BeginTurn(activeUnit->id);
}

Unit* CombatManager::GetActiveUnit() 
{
  if (m_turnQueue.empty() || m_currentTurnIndex >= m_turnQueue.size()) 
  {
    return nullptr;
  }
  return GetUnit(m_turnQueue[m_currentTurnIndex]);
}

Unit* CombatManager::GetUnit(Engine::Uuid id) 
{
  for (auto& unit : m_roster) 
  {
    if (unit.id == id) 
    {
      return &unit;
    }
  }
  return nullptr;
}