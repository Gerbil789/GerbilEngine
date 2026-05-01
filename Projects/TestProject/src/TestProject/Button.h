#pragma once

#include "Engine/Script/Script.h"



class Button : public Engine::Script
{
  SCRIPT_CLASS(Button)

public:
  PROPERTY(m_Offset, "Offset")
    float m_Offset = 15.0f;

  PROPERTY(m_PressSound, "Press Sound")
		Engine::AudioClip* m_PressSound = nullptr;

  PROPERTY(m_ReleaseSound, "Release Sound")
    Engine::AudioClip* m_ReleaseSound = nullptr;

  void OnTriggerEnter(Engine::Entity other) override;
  void OnTriggerExit(Engine::Entity other) override;
};