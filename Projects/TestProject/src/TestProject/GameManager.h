#pragma once

#include "Engine/Script/Script.h"

class GameManager : public Engine::Script
{
  SCRIPT_CLASS(GameManager)

public:
  PROPERTY(m_Shader, "Shader")
    Engine::Shader* m_Shader = nullptr;

  void OnStart() override;
  void OnUpdate() override;
};


