#include "enginepch.h"
#include "Engine/System/TransformSystem.h"
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
    if (!registry.any_of<component::Transform, component::WorldTransform>(entity)) return;

    auto& tc = registry.get<component::Transform>(entity);
    auto& wtc = registry.get<component::WorldTransform>(entity);
    bool needsUpdate = registry.any_of<tag::TransformDirty>(entity) || forceUpdate;

    if (needsUpdate)
    {
      const glm::mat4 localMatrix = TransformSystem::CalculateLocalMatrix(tc);
      wtc.worldMatrix = parentWorldMatrix * localMatrix;
      registry.remove<tag::TransformDirty>(entity);
    }

    const auto& rel = registry.get<component::Relationship>(entity);
    entt::entity child = rel.firstChild;

    while (child != entt::null)
    {
      entt::entity nextSibling = registry.get<component::Relationship>(child).nextSibling;

      UpdateEntityTransformRecursive(registry, child, wtc.worldMatrix, needsUpdate);

      child = nextSibling;
    }
  }

  void TransformSystem::Update(Scene& scene)
  {
    entt::entity rootEntity = scene.firstRoot;

    while (rootEntity != entt::null)
    {
      entt::entity nextRoot = scene.registry.get<component::Relationship>(rootEntity).nextSibling;

      UpdateEntityTransformRecursive(scene.registry, rootEntity, glm::mat4{ 1.0f });

      rootEntity = nextRoot;
    }
  }
}