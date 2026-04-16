#pragma once

#include "Engine/Script/Script.h"

class PlayerController : public Engine::Script
{
	SCRIPT_CLASS(PlayerController)

public:

  PROPERTY(m_MoveSpeed, "Move Speed")
    float m_MoveSpeed = 10.0f;

  PROPERTY(m_MouseSensitivity, "Mouse Sensitivity")
    float m_MouseSensitivity = 0.1f;

  PROPERTY(m_Ground, "Ground Level")
    float m_Ground = 0.0f;

  PROPERTY(m_GravityStrength, "Gravity Strength")
    float m_GravityStrength = 10.0f;

  PROPERTY(m_JumpStrength, "Jump Strength")
    float m_JumpStrength = 20.0f;

	void OnStart() override;
	void OnUpdate() override;
  void OnDestroy() override;
	void OnEvent(const Engine::Event& event) override;

private:
  float m_LastMouseX = 0.0f;
  float m_LastMouseY = 0.0f;
	float m_VelocityY = 0.0f;
};