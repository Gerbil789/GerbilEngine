#pragma once

#include "Engine/Script/Script.h" 
#include "Engine/Core/Log.h"

#include "Grid.h"

class BattleControllerScript : public Engine::Script 
{
  SCRIPT_CLASS(BattleControllerScript)

public:
  PROPERTY(m_GridWidth, "grid width")
    int m_GridWidth = 10;

	PROPERTY(m_GridHeight, "grid height")
		int m_GridHeight = 10;

  PROPERTY(m_TileMesh, "tile mesh")
    Engine::MeshHandle m_TileMesh;

  PROPERTY(m_TileMaterial, "tile material")
    Engine::MaterialHandle m_TileMaterial;

public:
  void OnStart() override;
  void OnUpdate() override;
  void OnDestroy() override;
};