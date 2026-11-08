#include "enginepch.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Core/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace engine
{
  const glm::mat4 TransformSystem::CalculateLocalMatrix(const component::Transform& transform)
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
      const glm::mat4 localMatrix = TransformSystem::CalculateLocalMatrix(tc);

			wtc.worldMatrix = parentWorldMatrix * localMatrix;
      registry.remove<tag::TransformDirty>(entity);
    }

    for (auto child : hc.children)
    {
      UpdateEntityTransformRecursive(registry, child, wtc.worldMatrix, needsUpdate);
    }
  }

	void TransformSystem::Update(Scene& scene)
	{
    for (entt::entity entity : scene.rootEntities)
    {
      UpdateEntityTransformRecursive(scene.registry, entity, glm::mat4{ 1.0f });
    }
	}
}