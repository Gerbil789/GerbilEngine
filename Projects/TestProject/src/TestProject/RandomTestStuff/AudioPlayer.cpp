#include "AudioPlayer.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Components.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Audio/AudioClip.h"

void AudioPlayer::OnStart()
{
  if (m_PlayOnAwake && m_Clip)
  {
    auto& transform = GetComponent<Engine::TransformComponent>();
		glm::vec3 localPosition = transform.position;
    glm::vec3 worldPosition =  transform.worldMatrix * glm::vec4(localPosition, 1.0f);

    Engine::Audio::Play3D(m_Clip, worldPosition);
  }
}

void AudioPlayer::OnUpdate()
{
  if (!m_Clip) return;

  auto& transform = GetComponent<Engine::TransformComponent>();
  glm::vec3 localPosition = transform.position;
  glm::vec3 worldPosition = transform.worldMatrix * glm::vec4(localPosition, 1.0f);

  Engine::Audio::SetSourcePosition(m_Clip, worldPosition);
}
