#include "enginepch.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/HierarchySystem.h"

namespace Engine
{
  void Entity::SetParent(Entity newParent, bool keepWorld)
  {
    HierarchySystem::SetParent(*m_Registry, m_Handle, newParent.Handle(), keepWorld);
  }

  void Entity::AddChild(Entity child, bool keepWorld)
  {
    HierarchySystem::AddChild(*m_Registry, m_Handle, child.Handle(), keepWorld);
  }

  std::vector<Entity> Entity::GetChildren() const
  {
    std::vector<Entity> children;
    for (auto e : HierarchySystem::GetChildren(*m_Registry, m_Handle))
    {
      children.push_back(Entity{ e, m_Registry });
    }
    return children;
  }

  void Entity::Destroy()
  {
    if (m_Registry && m_Registry->valid(m_Handle))
    {
      m_Registry->destroy(m_Handle);
    }

    m_Handle = entt::null;
    m_Registry = nullptr;
  }
}