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

  PROPERTY(m_PlayOnAwake, "Play On Awake")
    bool m_PlayOnAwake = false;

  void OnStart() override
  {
    if (m_PlayOnAwake && m_Clip)
    {
      const auto& pos = Self.GetComponent<Engine::TransformComponent>().position;
      Engine::Audio::Play3D(m_Clip, pos.x, pos.y, pos.z);
    }
	}

  void OnUpdate() override
  {
    if (!m_Clip) return;

    if(Engine::Audio::IsPlaying(m_Clip))
    {
      const auto& pos = Self.GetComponent<Engine::TransformComponent>().position;
      Engine::Audio::SetSourcePosition(m_Clip, pos.x, pos.y, pos.z);
		}


  //  if (Engine::Input::IsKeyPressedOnce(Engine::KeyCode::Space) && !Engine::Audio::IsPlaying(m_Clip))
  //  {
  //    auto pos = Self.GetComponent<Engine::TransformComponent>().position;

  //    Engine::Audio::Play3D(m_Clip, pos.x, pos.y, pos.z);
		//}
  }
};