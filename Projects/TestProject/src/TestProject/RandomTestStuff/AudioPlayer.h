#pragma once

#include "Engine/Script/Script.h"

class AudioPlayer : public Engine::Script
{
  SCRIPT_CLASS(AudioPlayer)

public:

  PROPERTY(m_Clip, "Audio Clip")
		Engine::AudioClipHandle m_Clip;

  PROPERTY(m_PlayOnAwake, "Play On Awake")
    bool m_PlayOnAwake = false;

  void OnStart() override;


  void OnUpdate() override;
};