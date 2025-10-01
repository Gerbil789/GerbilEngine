#include "enginepch.h"
#include "Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/HierarchySystem.h"

namespace Engine
{
  void Entity::SetParent(Entity newParent, bool keepWorld)
  {
    HierarchySystem::SetParent(m_Scene->m_Registry, m_EntityHandle, entt::entity(newParent), keepWorld);
  }

  void Entity::AddChild(Entity child, bool keepWorld)
  {
    HierarchySystem::AddChild(m_Scene->m_Registry, m_EntityHandle, child, keepWorld);
  }

  std::vector<Entity> Entity::GetChildren() const
  {
    std::vector<Entity> children;
    for (auto e : HierarchySystem::GetChildren(m_Scene->m_Registry, m_EntityHandle))
      children.push_back(Entity(e, m_Scene));
    return children;
  }

  void Entity::Destroy()
  {
		HierarchySystem::DestroyEntity(m_Scene->m_Registry, m_EntityHandle);
		m_EntityHandle = entt::null;
  }
}