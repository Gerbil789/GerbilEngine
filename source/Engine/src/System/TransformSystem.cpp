#include "enginepch.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
  const glm::mat4 TransformSystem::CalculateLocalPositionMatrix(const TransformComponent& transform)
  {
    return glm::translate(glm::mat4(1.0f), transform.position) * glm::toMat4(glm::quat(transform.rotation)) * glm::scale(glm::mat4(1.0f), transform.scale);
  }


  void UpdateEntityTransformRecursive(entt::registry& registry, entt::entity entity, const glm::mat4& parentWorldMatrix, bool forceUpdate = false)
  {
    if (!registry.any_of<TransformComponent>(entity)) return;

    auto& tc = registry.get<TransformComponent>(entity);
    auto& hc = registry.get<HierarchyComponent>(entity);
		auto& wtc = registry.get<WorldTransformComponent>(entity);
    bool needsUpdate = registry.any_of<TransformDirty>(entity) || forceUpdate;

    if (needsUpdate)
    {
      const glm::mat4 localMatrix = TransformSystem::CalculateLocalPositionMatrix(tc);

			wtc.worldMatrix = parentWorldMatrix * localMatrix;
      registry.remove<TransformDirty>(entity);
    }

    for (auto child : hc.children)
    {
      UpdateEntityTransformRecursive(registry, child, wtc.worldMatrix, needsUpdate);
    }
  }

	void TransformSystem::Update(SceneAsset& scene)
	{
		auto& registry = scene.GetRegistry();

    for (entt::entity entity : scene.GetRootEntities())
    {
      UpdateEntityTransformRecursive(registry, entity, glm::mat4{ 1.0f });
    }
	}
}