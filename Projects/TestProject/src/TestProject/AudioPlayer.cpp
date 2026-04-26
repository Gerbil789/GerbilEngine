#include "AudioPlayer.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Components.h"

void AudioPlayer::OnStart()
{
  if (m_PlayOnAwake && m_Clip)
  {
    const auto& pos = Self.Get<Engine::TransformComponent>().position;
    Engine::Audio::Play3D(m_Clip, pos.x, pos.y, pos.z);
  }
}

void AudioPlayer::OnUpdate()
{
  if (!m_Clip) return;

  if (Engine::Audio::IsPlaying(m_Clip))
  {
    const auto& pos = Self.Get<Engine::TransformComponent>().position;
    Engine::Audio::SetSourcePosition(m_Clip, pos.x, pos.y, pos.z);
  }
}
