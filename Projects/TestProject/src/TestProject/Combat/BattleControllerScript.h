#pragma once

#include "Engine/Script/Script.h" 
#include "Engine/Core/Log.h"

#include "Grid.h"
#include "CombatManager.h"
#include "PlayerCombatController.h"
#include "AICombatController.h"

class BattleControllerScript : public Engine::Script 
{
  SCRIPT_CLASS(BattleControllerScript)

public:
  PROPERTY(m_GridWidth, "Grid Width")
    int m_GridWidth = 10;

	PROPERTY(m_GridHeight, "Grid Height")
		int m_GridHeight = 10;

public:
  void OnStart() override
  {
    m_grid = new Grid(m_GridWidth, m_GridHeight);
    m_playerCtrl = new PlayerController(m_combatManager, m_grid);
    m_aiCtrl = new AIController(m_combatManager, m_grid);
    m_combatManager = new CombatManager(m_grid, m_playerCtrl, m_aiCtrl);

    SetupLevel();

		LOG_INFO("BattleControllerScript created and initialized.");

    m_combatManager->StartBattle();
  }

  void OnUpdate() override 
  {
    if (m_combatManager)
    {
      m_combatManager->Update();
    }
  }

  void OnDestroy() override 
  {
    delete m_aiCtrl;
    delete m_playerCtrl;
    delete m_combatManager;
    delete m_grid;

		LOG_INFO("BattleControllerScript destroyed and cleaned up.");
  }

private:
  Grid* m_grid = nullptr;
  CombatManager* m_combatManager = nullptr;

  // Storing controllers here ensures they stay alive for the whole battle
  PlayerController* m_playerCtrl = nullptr;
  AIController* m_aiCtrl = nullptr;

  void SetupLevel() 
  {
    //Engine::Uuid entityId = InstantiatePrefab("Prefabs/HumanArcher");

    // 2. Tell the game logic that this unit exists
    //Unit archer = UnitFactory::CreateUnit("Archer", entityId);
    //archer.team = Team::Attacker;

    // 3. Add to the manager
    //m_combatManager->AddUnitToBattle(archer);
  }
};