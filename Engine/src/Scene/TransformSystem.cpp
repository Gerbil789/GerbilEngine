#include "enginepch.h"
#include "Engine/Scene/TransformSystem.h"
#include "Engine/Scene/SceneManager.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
  // Recursive helper function inside the system
  void UpdateWorldMatrix(entt::registry& registry, entt::entity entity)
  {
    auto& transform = registry.get<TransformComponent>(entity);

    if (transform.parent == entt::null)
    {
      transform.worldMatrix = transform.localMatrix;
    }
    else
    {
      // Ensure parent is updated first
      UpdateWorldMatrix(registry, transform.parent);
      auto& parentTransform = registry.get<TransformComponent>(transform.parent);

      transform.worldMatrix = parentTransform.worldMatrix * transform.localMatrix;
    }

    transform.isDirty = false;
  }

  void TransformSystem::Update()
  {
		Scene& scene = SceneManager::GetActiveScene();
		entt::registry& registry = scene.GetRegistry();

    // update local matrices
    auto view = registry.view<TransformComponent>();
    for (auto entity : view)
    {
      auto& transform = view.get<TransformComponent>(entity);
      if (!transform.isDirty)
      {
        continue;
      }

      transform.localMatrix = glm::translate(glm::mat4(1.0f), transform.position) * glm::toMat4(glm::quat(glm::radians(transform.rotation))) * glm::scale(glm::mat4(1.0f), transform.scale);
    }

    for (auto entity : view)
    {
      UpdateWorldMatrix(registry, entity);
    }
  }


}