#include "enginepch.h"
#include "Engine/Scene/HierarchySystem.h"
#include "Engine/Scene/Components.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Engine
{
  namespace
  {
    void Detach(entt::registry& registry, entt::entity child)
    {
      auto& transform = registry.get<TransformComponent>(child);
      if (transform.parent == entt::null)
        return;

      auto& parentTransform = registry.get<TransformComponent>(transform.parent);

      // If this was the first child, update parent's pointer
      if (parentTransform.firstChild == child)
        parentTransform.firstChild = transform.nextSibling;

      // Fix sibling links
      if (transform.prevSibling != entt::null)
        registry.get<TransformComponent>(transform.prevSibling).nextSibling = transform.nextSibling;
      if (transform.nextSibling != entt::null)
        registry.get<TransformComponent>(transform.nextSibling).prevSibling = transform.prevSibling;

      // Clear child links
      transform.parent = entt::null;
      transform.prevSibling = entt::null;
      transform.nextSibling = entt::null;
    }

    // Internal: Attach child under parent at the front of the list
    void Attach(entt::registry& registry, entt::entity parent, entt::entity child)
    {
      auto& parentTransform = registry.get<TransformComponent>(parent);
      auto& childTransform = registry.get<TransformComponent>(child);

      childTransform.parent = parent;
      childTransform.prevSibling = entt::null;
      childTransform.nextSibling = parentTransform.firstChild;

      if (parentTransform.firstChild != entt::null)
      {
        auto& first = registry.get<TransformComponent>(parentTransform.firstChild);
        first.prevSibling = child;
      }

      parentTransform.firstChild = child;
    }
  }

  bool IsDescendant(entt::registry& registry, entt::entity ancestor, entt::entity descendant)
  {
    if (ancestor == entt::null || descendant == entt::null)
    {
      return false;
    }

    auto* tc = registry.try_get<TransformComponent>(descendant);
    while (tc && tc->parent != entt::null)
    {
      if (tc->parent == ancestor)
        return true;

      tc = registry.try_get<TransformComponent>(tc->parent);
    }
    return false;
  }

  // ---- Public API ----

	void HierarchySystem::SetParent(entt::registry& registry, entt::entity child, entt::entity newParent, bool keepWorld)
	{
    if (IsDescendant(registry, child, newParent))
    {
			return; // prevent cycles
    }

    Detach(registry, child);
    if (newParent != entt::null)
    {
      Attach(registry, newParent, child);
    }

    if (keepWorld)
    {
      glm::mat4 worldMatrix = registry.get<TransformComponent>(child).GetWorldMatrix(registry);

      // Recalculate local transform so world stays same
      auto& childTransform = registry.get<TransformComponent>(child);
      glm::mat4 parentWorld = glm::mat4(1.0f);
      if (childTransform.parent != entt::null)
        parentWorld = registry.get<TransformComponent>(childTransform.parent).GetWorldMatrix(registry);

      glm::mat4 local = glm::inverse(parentWorld) * worldMatrix;

      // Decompose matrix -> Position/Rotation/Scale (helper needed)
      glm::vec3 skew;
      glm::vec4 perspective;
      glm::quat rot;
      glm::vec3 trans, scale;
      glm::decompose(local, scale, rot, trans, skew, perspective);

      childTransform.position = trans;
      childTransform.rotation = glm::degrees(glm::eulerAngles(rot));
      childTransform.scale = scale;
    }
	}

	void HierarchySystem::RemoveParent(entt::registry& registry, entt::entity child, bool keepWorld)
	{
    SetParent(registry, child, entt::null, keepWorld);
	}

	void HierarchySystem::AddChild(entt::registry& registry, entt::entity parent, entt::entity child, bool keepWorld)
	{
    SetParent(registry, child, parent, keepWorld);
	}

	void HierarchySystem::RemoveChild(entt::registry& registry, entt::entity parent, entt::entity child)
	{
    auto& childTransform = registry.get<TransformComponent>(child);
    if (childTransform.parent == parent)
      Detach(registry, child);
	}

	std::vector<entt::entity> HierarchySystem::GetChildren(entt::registry& registry, entt::entity parent)
	{
    std::vector<entt::entity> result;
    auto& parentTransform = registry.get<TransformComponent>(parent);

    entt::entity child = parentTransform.firstChild;
    while (child != entt::null)
    {
      result.push_back(child);
      child = registry.get<TransformComponent>(child).nextSibling;
    }

    return result;
	}

	entt::entity HierarchySystem::GetParent(entt::registry& registry, entt::entity child)
	{
    return registry.get<TransformComponent>(child).parent;
	}

  void HierarchySystem::DestroyEntity(entt::registry& registry, entt::entity entity)
  {
    if (!registry.valid(entity))
      return;

    Detach(registry, entity);

    // Recursively destroy children
    auto& transform = registry.get<TransformComponent>(entity);
    entt::entity child = transform.firstChild;
    while (child != entt::null)
    {
      entt::entity next = registry.get<TransformComponent>(child).nextSibling;
      DestroyEntity(registry, child);
      child = next;
    }

    // Finally destroy self
		registry.destroy(entity);
  }


}


