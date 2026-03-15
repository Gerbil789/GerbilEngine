#include "enginepch.h"
#include "Engine/Scene/Components.h"
#include "Engine/Audio/Audio.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Engine/Graphics/Material.h"

namespace Engine
{
  MeshComponent::MeshComponent()
  {
    SetMaterial(0, Materials::GetDefault());
  }


  glm::mat4 TransformComponent::GetLocalMatrix() const
  {
    return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);
  }

  glm::mat4 TransformComponent::GetWorldMatrix() const
  {
    glm::mat4 local = GetLocalMatrix();
    if (parent)
    {
			const auto& parentTransform = parent.Get<TransformComponent>();
      return parentTransform.GetWorldMatrix() * local;
    }
    return local;
  }
}