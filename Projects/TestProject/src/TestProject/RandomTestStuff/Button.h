#pragma once

#include "Engine/Script/Script.h"

class Button : public Engine::Script
{
  SCRIPT_CLASS(Button)

public:
  PROPERTY(m_Speed, "Speed")
    float m_Speed = 10.0f;

  PROPERTY(m_PressSound, "Press Sound")
    Engine::AudioClipHandle m_PressSound;

  PROPERTY(m_ReleaseSound, "Release Sound")
    Engine::AudioClipHandle m_ReleaseSound;

	void OnUpdate() override;
  void OnTriggerEnter(Engine::Entity other) override;
  void OnTriggerExit(Engine::Entity other) override;

private:
	bool m_Pressed = false;
};