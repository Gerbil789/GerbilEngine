#include "enginepch.h"
#include "Engine/Scene/TransformSystem.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
  void UpdateEntityTransformRecursive(entt::registry& registry, entt::entity entity, const glm::mat4& parentWorldMatrix, bool forceUpdate = false)
  {
    if (!registry.any_of<TransformComponent>(entity)) return;

    auto& tc = registry.get<TransformComponent>(entity);
    auto& hc = registry.get<HierarchyComponent>(entity);
		auto& wtc = registry.get<WorldTransformComponent>(entity);
    bool needsUpdate = registry.any_of<TransformDirty>(entity) || forceUpdate;

    if (needsUpdate)
    {
      //const glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), tc.position) * glm::toMat4(glm::quat(glm::radians(tc.rotation))) * glm::scale(glm::mat4(1.0f), tc.scale);
      const glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), tc.position) * glm::toMat4(glm::quat(tc.rotation)) * glm::scale(glm::mat4(1.0f), tc.scale);

			wtc.worldMatrix = parentWorldMatrix * localMatrix;
      registry.remove<TransformDirty>(entity);
    }

    for (auto child : hc.children)
    {
      UpdateEntityTransformRecursive(registry, child, wtc.worldMatrix, needsUpdate);
    }
  }

	void TransformSystem::Update(Scene& scene)
	{
		auto& registry = scene.GetRegistry();

    for (entt::entity entity : scene.GetRootEntities())
    {
      UpdateEntityTransformRecursive(registry, entity, glm::mat4{ 1.0f });
    }
	}
}