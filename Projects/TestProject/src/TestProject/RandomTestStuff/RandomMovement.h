#pragma once

#include "Engine/Script/Script.h"

class RandomMovement : public Engine::Script
{
	SCRIPT_CLASS(RandomMovement)

public:
	PROPERTY(m_Speed, "Speed")
		float m_Speed = 10.0f;

	PROPERTY(m_MinScale, "Min Scale")
		float m_MinScale = 0.5f;

	PROPERTY(m_MaxScale, "Max Scale")
		float m_MaxScale = 1.5f;

	void OnCreate() override;
	void OnUpdate() override;

private:
	float m_ElapsedTime = 0.0f;
};