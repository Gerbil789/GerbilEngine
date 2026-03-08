#pragma once

#include "Engine/Script/Script.h"

class GameManager : public Engine::Script
{
  SCRIPT_CLASS(GameManager)

public:

  PROPERTY(m_Mesh, "Mesh")
    Engine::Mesh* m_Mesh = nullptr;

  void OnStart() override;
  void OnUpdate() override;
};


