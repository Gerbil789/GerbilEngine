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

  PROPERTY(m_Texture, "Texture test")
    Engine::Texture2D* m_Texture = nullptr;

  PROPERTY(m_Clip, "Audio test")
    Engine::AudioClip* m_Clip = nullptr;

	void OnCreate() override;
	void OnUpdate() override;
};