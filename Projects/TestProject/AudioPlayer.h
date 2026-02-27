#pragma once

#include "Engine/Script/Script.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Input.h"

class AudioPlayer : public Engine::Script
{
  SCRIPT_CLASS(AudioPlayer)

public:

  PROPERTY(m_Clip, "Audio Clip")
    Engine::AudioClip* m_Clip = nullptr;


  void OnUpdate() override
  {
    if (!m_Clip) return;


    Self.


    if (Engine::Input::IsKeyPressed(Engine::Key::Space) && !Engine::Audio::IsPlaying(m_Clip))
    {
      Engine::Audio::Play(m_Clip);
		}
  }
};