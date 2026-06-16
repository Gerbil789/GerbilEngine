#pragma once

#include "Engine/Script/Script.h"

class GameManager : public Engine::Script
{
  SCRIPT_CLASS(GameManager)

public:
  PROPERTY(m_Shader, "Shader")
    Engine::ShaderHandle m_Shader;

  void OnStart() override;
  void OnUpdate() override;
};


