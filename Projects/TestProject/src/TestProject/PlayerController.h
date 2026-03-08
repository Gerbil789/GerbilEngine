#pragma once

#include "Engine/Script/Script.h"

class Bullet : public Engine::Script
{
  SCRIPT_CLASS(Bullet)

public:
	Bullet(glm::vec3 direction) : m_Direction(direction) {}
  float m_Speed = 10.0f;
  float m_Time = 10.0f;
	glm::vec3 m_Direction;

  void OnUpdate() override;
};


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
    float m_JumpStrength = 20.0f;

  PROPERTY(m_Sound, "Shoot Sound")
    Engine::AudioClip* m_Sound = nullptr;

  PROPERTY(m_BulletMesh, "Bullet Mesh")
    Engine::Mesh* m_BulletMesh = nullptr;

  PROPERTY(m_BulletMaterial, "Bullet Material")
    Engine::Material* m_BulletMaterial = nullptr;

	void OnCreate() override;
	void OnUpdate() override;
};


