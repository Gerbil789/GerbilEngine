#include "enginepch.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace engine
{
  const glm::mat4 TransformSystem::CalculateLocalPositionMatrix(const component::Transform& transform)
  {
    return glm::translate(glm::mat4(1.0f), transform.position) * glm::toMat4(glm::quat(transform.rotation)) * glm::scale(glm::mat4(1.0f), transform.scale);
  }


  void UpdateEntityTransformRecursive(entt::registry& registry, entt::entity entity, const glm::mat4& parentWorldMatrix, bool forceUpdate = false)
  {
    if (!registry.any_of<component::Transform>(entity)) return;

    auto& tc = registry.get<component::Transform>(entity);
    auto& hc = registry.get<component::Hierarchy>(entity);
		auto& wtc = registry.get<component::WorldTransform>(entity);
    bool needsUpdate = registry.any_of<tag::TransformDirty>(entity) || forceUpdate;

    if (needsUpdate)
    {
      const glm::mat4 localMatrix = TransformSystem::CalculateLocalPositionMatrix(tc);

			wtc.worldMatrix = parentWorldMatrix * localMatrix;
      registry.remove<tag::TransformDirty>(entity);
    }

    for (auto child : hc.children)
    {
      UpdateEntityTransformRecursive(registry, child, wtc.worldMatrix, needsUpdate);
    }
  }

	void TransformSystem::Update(entt::registry& registry, const std::vector<entt::entity>& rootEntities)
	{
    for (entt::entity entity : rootEntities)
    {
      UpdateEntityTransformRecursive(registry, entity, glm::mat4{ 1.0f });
    }
	}
}