#include "enginepch.h"
#include "Engine/Scene/Components.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
  glm::mat4 TransformComponent::GetLocal() const
  {
    return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);
  }

  glm::mat4 TransformComponent::GetWorld() const
  {
    glm::mat4 local = GetLocal();
    if (parent)
    {
			const auto& parentTransform = parent.Get<TransformComponent>();
      return parentTransform.GetWorld() * local;
    }
    return local;
  }
}