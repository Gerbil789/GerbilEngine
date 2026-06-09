#pragma once

#include "Engine/Script/Script.h"

class Button : public Engine::Script
{
  SCRIPT_CLASS(Button)

public:
  PROPERTY(m_Speed, "Speed")
    float m_Speed = 10.0f;

  PROPERTY(m_PressSound, "Press Sound")
		Engine::AudioClip* m_PressSound = nullptr;

  PROPERTY(m_ReleaseSound, "Release Sound")
    Engine::AudioClip* m_ReleaseSound = nullptr;

	void OnUpdate() override;
  void OnTriggerEnter(entt::entity other) override;
  void OnTriggerExit(entt::entity other) override;

private:
	bool m_Pressed = false;
};