#include "enginepch.h"
#include "Engine/Scene/Components.h"
#include "Engine/Audio/Audio.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
  glm::mat4 TransformComponent::GetLocalMatrix() const
  {
    return glm::translate(glm::mat4(1.0f), position) *
      glm::toMat4(glm::quat(glm::radians(rotation))) *
      glm::scale(glm::mat4(1.0f), scale);
  }

  glm::mat4 TransformComponent::GetWorldMatrix(const entt::registry& registry) const
  {
    glm::mat4 local = GetLocalMatrix();
    if (parent != entt::null)
    {
      const auto& parentTransform = registry.get<TransformComponent>(parent);
      return parentTransform.GetWorldMatrix(registry) * local;
    }
    return local;
  }

  void AudioSourceComponent::Play()
  {
    Audio::Play(clip);
	}

  void AudioSourceComponent::Stop()
  {
    Audio::Stop(clip);
	}

  void AudioSourceComponent::SetVolume(float volume)
  {
    Audio::SetVolume(clip, volume);
  }
  void AudioSourceComponent::SetLooping(bool loop)
  {
    Audio::SetLooping(clip, loop);
  }
}