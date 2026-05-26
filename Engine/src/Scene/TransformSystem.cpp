#include "enginepch.h"
#include "Engine/Scene/TransformSystem.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Components.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
  void UpdateWorldMatrix(entt::registry& registry, entt::entity entity)
  {
    auto& transform = registry.get<TransformComponent>(entity);

    if (transform.parent == entt::null)
    {
      transform.worldMatrix = transform.localMatrix;
    }
    else
    {
      UpdateWorldMatrix(registry, transform.parent);
      auto& parentTransform = registry.get<TransformComponent>(transform.parent);

      transform.worldMatrix = parentTransform.worldMatrix * transform.localMatrix;
    }
  }

  void TransformSystem::Update()
  {
    Scene& scene = SceneManager::GetActiveScene();
    entt::registry& registry = scene.GetRegistry();

		auto view = registry.view<TransformComponent>();

    for (const auto entity : view)
    {
      auto& transform = registry.get<TransformComponent>(entity);
      transform.localMatrix = glm::translate(glm::mat4(1.0f), transform.position) * glm::toMat4(glm::quat(glm::radians(transform.rotation))) *glm::scale(glm::mat4(1.0f), transform.scale);
    }

    for (const auto entity : view)
    {
      UpdateWorldMatrix(registry, entity);
    }
  }
}