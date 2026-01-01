#include "enginepch.h"
#include "Components.h"
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
}