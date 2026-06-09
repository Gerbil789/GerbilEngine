#pragma once

#include "Engine/Script/Script.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"


class AudioPlayer : public Engine::Script
{
  SCRIPT_CLASS(AudioPlayer)

public:

  PROPERTY(m_Clip, "Audio Clip")
    Engine::AudioClip* m_Clip = nullptr;

  PROPERTY(m_PlayOnAwake, "Play On Awake")
    bool m_PlayOnAwake = false;

  void OnStart() override;


  void OnUpdate() override;
};