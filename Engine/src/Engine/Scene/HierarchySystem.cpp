#include "enginepch.h"
#include "HierarchySystem.h"
#include "Engine/Scene/Components.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Engine
{
  namespace
  {
    void Detach(entt::registry& registry, entt::entity child)
    {
      auto& transform = registry.get<TransformComponent>(child);
      if (transform.Parent == entt::null)
        return;

      auto& parentTransform = registry.get<TransformComponent>(transform.Parent);

      // If this was the first child, update parent's pointer
      if (parentTransform.FirstChild == child)
        parentTransform.FirstChild = transform.NextSibling;

      // Fix sibling links
      if (transform.PrevSibling != entt::null)
        registry.get<TransformComponent>(transform.PrevSibling).NextSibling = transform.NextSibling;
      if (transform.NextSibling != entt::null)
        registry.get<TransformComponent>(transform.NextSibling).PrevSibling = transform.PrevSibling;

      // Clear child links
      transform.Parent = entt::null;
      transform.PrevSibling = entt::null;
      transform.NextSibling = entt::null;
    }

    // Internal: Attach child under parent at the front of the list
    void Attach(entt::registry& registry, entt::entity parent, entt::entity child)
    {
      auto& parentTransform = registry.get<TransformComponent>(parent);
      auto& childTransform = registry.get<TransformComponent>(child);

      childTransform.Parent = parent;
      childTransform.PrevSibling = entt::null;
      childTransform.NextSibling = parentTransform.FirstChild;

      if (parentTransform.FirstChild != entt::null)
      {
        auto& first = registry.get<TransformComponent>(parentTransform.FirstChild);
        first.PrevSibling = child;
      }

      parentTransform.FirstChild = child;
    }
  }

  bool IsDescendant(entt::registry& registry, entt::entity ancestor, entt::entity descendant)
  {
    if (ancestor == entt::null || descendant == entt::null)
    {
      return false;
    }

    auto* tc = registry.try_get<TransformComponent>(descendant);
    while (tc && tc->Parent != entt::null)
    {
      if (tc->Parent == ancestor)
        return true;

      tc = registry.try_get<TransformComponent>(tc->Parent);
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
      if (childTransform.Parent != entt::null)
        parentWorld = registry.get<TransformComponent>(childTransform.Parent).GetWorldMatrix(registry);

      glm::mat4 local = glm::inverse(parentWorld) * worldMatrix;

      // Decompose matrix -> Position/Rotation/Scale (helper needed)
      glm::vec3 skew;
      glm::vec4 perspective;
      glm::quat rot;
      glm::vec3 trans, scale;
      glm::decompose(local, scale, rot, trans, skew, perspective);

      childTransform.Position = trans;
      childTransform.Rotation = glm::degrees(glm::eulerAngles(rot));
      childTransform.Scale = scale;
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
    if (childTransform.Parent == parent)
      Detach(registry, child);
	}

	std::vector<entt::entity> HierarchySystem::GetChildren(entt::registry& registry, entt::entity parent)
	{
    std::vector<entt::entity> result;
    auto& parentTransform = registry.get<TransformComponent>(parent);

    entt::entity child = parentTransform.FirstChild;
    while (child != entt::null)
    {
      result.push_back(child);
      child = registry.get<TransformComponent>(child).NextSibling;
    }

    return result;
	}

	entt::entity HierarchySystem::GetParent(entt::registry& registry, entt::entity child)
	{
    return registry.get<TransformComponent>(child).Parent;
	}

  void HierarchySystem::DestroyEntity(entt::registry& registry, entt::entity entity)
  {
    if (!registry.valid(entity))
      return;

    Detach(registry, entity);

    // Recursively destroy children
    auto& transform = registry.get<TransformComponent>(entity);
    entt::entity child = transform.FirstChild;
    while (child != entt::null)
    {
      entt::entity next = registry.get<TransformComponent>(child).NextSibling;
      DestroyEntity(registry, child);
      child = next;
    }

    // Finally destroy self
		registry.destroy(entity);
  }


}


