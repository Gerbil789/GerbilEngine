#pragma once
#include <vector>
#include "Unit.h"
#include "Grid.h"
#include "ICombatController.h"
#include <map>

enum class TurnState 
{
  Idle,           // Waiting for the turn to start, or enemy is acting
  ChoosingAction, // Player is looking at the UI/Menu
  Targeting,      // Player clicked "Fireball" and is moving the mouse
  Executing,      // The animation/projectile is playing
  AnimatingMove   // Unit is currently walking across the grid
};


class CombatManager 
{
public:
  CombatManager(ICombatController* attackerControl, ICombatController* defenderControl);

  void AddUnitToBattle(const Unit& unit);
  void StartBattle();
	void Update();

  void NextTurn();

  Unit* GetActiveUnit();
  const std::vector<Engine::Uuid>& GetTurnQueue() const { return m_turnQueue; }

private:
  std::map<Team, ICombatController*> m_controllers;
  ICombatController* m_activeController = nullptr;

  std::vector<Unit> m_roster;
  std::vector<Engine::Uuid> m_turnQueue;

  int m_currentTurnIndex = 0;
  int m_roundNumber = 1;

  void RollInitiative();
  Unit* GetUnit(Engine::Uuid id);
};