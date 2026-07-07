#include "enginepch.h"
#include "Engine/Scene/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
  void UpdateWorldMatrix(TransformComponent& transform)
  {
    if (!transform.parent.IsValid())
    {
      transform.worldMatrix = transform.localMatrix;
    }
    else
    {
      auto& parentTransform = transform.parent.GetComponent<TransformComponent>();
      UpdateWorldMatrix(parentTransform);
      transform.worldMatrix = parentTransform.worldMatrix * transform.localMatrix;
    }
  }

  void TransformComponent::UpdateMatrix()
  {
    localMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);
		UpdateWorldMatrix(*this);
  }
}