#pragma once

//#include "Engine/Script/Script.h"
//
//class FPSPlayerController : public Engine::Script
//{
//	SCRIPT_CLASS(FPSPlayerController)
//
//public:
//
//  PROPERTY(m_moveSpeed, "Move Speed")
//    float m_moveSpeed = 10.0f;
//
//  PROPERTY(m_mouseSensitivity, "Mouse Sensitivity")
//    float m_mouseSensitivity = 0.1f;
//
//  PROPERTY(m_ground, "Ground Level")
//    float m_ground = 0.0f;
//
//  PROPERTY(m_gravityStrength, "Gravity Strength")
//    float m_gravityStrength = 10.0f;
//
//  PROPERTY(m_jumpStrength, "Jump Strength")
//    float m_jumpStrength = 20.0f;
//
//	void OnStart() override;
//	void OnUpdate() override;
//  void OnDestroy() override;
//	void OnEvent(const Engine::Event& event) override;
//
//private:
//  float m_lastMouseX = 0.0f;
//  float m_lastMouseY = 0.0f;
//	float m_velocityY = 0.0f;
//};