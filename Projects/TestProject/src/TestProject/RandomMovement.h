#pragma once

#include "Engine/Script/Script.h"

class RandomMovement : public Engine::Script
{
public:
	float m_Speed = 10.0f;
	float m_MinScale = 0.5f;
	float m_MaxScale = 1.5f;

	void OnCreate() override;
	void OnUpdate(float delta) override;

private:
	float m_ElapsedTime = 0.0f;
};