#include "AudioPlayer.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Components.h"

void AudioPlayer::OnStart()
{
  if (m_PlayOnAwake && m_Clip)
  {
    Engine::Audio::Play3D(m_Clip, Self.Get<Engine::TransformComponent>().position);
  }
}

void AudioPlayer::OnUpdate()
{
  if (!m_Clip) return;

  if (Engine::Audio::IsPlaying(m_Clip))
  {
    Engine::Audio::SetSourcePosition(m_Clip, Self.Get<Engine::TransformComponent>().position);
  }
}
