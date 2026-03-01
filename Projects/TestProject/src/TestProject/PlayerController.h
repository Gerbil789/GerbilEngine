#pragma once

#include "Engine/Script/Script.h"

class PlayerController : public Engine::Script
{
	SCRIPT_CLASS(PlayerController)

public:

  PROPERTY(m_MoveSpeed, "Move Speed")
    float m_MoveSpeed = 10.0f;

  PROPERTY(m_Ground, "Ground Level")
    float m_Ground = -2.0f;

  PROPERTY(m_GravityStrength, "Gravity Strength")
    float m_GravityStrength = 10.0f;

  PROPERTY(m_JumpStrength, "Jump Strength")
    float m_JumpStrength = 2.0f;

  PROPERTY(m_Sound, "Jump Sound")
    Engine::AudioClip* m_Sound = nullptr;

	void OnCreate() override;
	void OnUpdate() override;
};