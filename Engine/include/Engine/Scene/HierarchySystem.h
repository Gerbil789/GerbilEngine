#pragma once

#include <entt.hpp>

namespace Engine::HierarchySystem
{
  void SetParent(entt::registry& registry, entt::entity child, entt::entity newParent, bool keepWorld);
  void RemoveParent(entt::registry& registry, entt::entity child, bool keepWorld);
  void AddChild(entt::registry& registry, entt::entity parent, entt::entity child, bool keepWorld);
  void RemoveChild(entt::registry& registry, entt::entity parent, entt::entity child);
  std::vector<entt::entity> GetChildren(entt::registry& registry, entt::entity parent);
  entt::entity GetParent(entt::registry& registry, entt::entity child);
	void DestroyEntity(entt::registry& registry, entt::entity entity);
}