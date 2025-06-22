#include "enginepch.h"
#include "Entity.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	//void Entity::SetParent(Entity newParent)
	//{
	//	if (m_EntityHandle == newParent || IsDescendant(m_EntityHandle, newParent))
	//		return; // Disallow cycles

	//	// Remove from old parent's children list
	//	if (m_Registry->all_of<HierarchyComponent>(m_EntityHandle)) {
	//		auto oldParent = m_Registry->get<ParentComponent>(m_EntityHandle).parent;
	//		if (m_Registry->valid(oldParent) && m_Registry->all_of<ChildrenComponent>(oldParent)) {
	//			auto& oldChildren = m_Registry->get<ChildrenComponent>(oldParent).children;
	//			std::erase(oldChildren, m_EntityHandle);
	//		}
	//	}

	//	// Set new parent
	//	if (!m_Registry->all_of<ParentComponent>(m_EntityHandle))
	//		m_Registry->emplace<ParentComponent>(m_EntityHandle);

	//	m_Registry->get<ParentComponent>(m_EntityHandle).parent = newParent;

	//	// Add to new parent's children list
	//	if (!m_Registry->all_of<ChildrenComponent>(newParent))
	//		m_Registry->emplace<ChildrenComponent>(newParent);

	//	auto& newChildren = m_Registry->get<ChildrenComponent>(newParent).children;
	//	newChildren.push_back(m_EntityHandle);
	//}

	//void Entity::AddChild(Entity child)
	//{
	//	child.SetParent(*this);
	//}

	//void Entity::RemoveChild(Entity child)
	//{
	//	if(!child) return; // Return if the child is invalid (null entity)
	//	auto& children = GetComponent<RelationshipComponent>().Children;
	//	auto it = std::remove(children.begin(), children.end(), child);
	//	if (it != children.end())
	//	{
	//		children.erase(it);
	//	}

	//	// Update the child's parent reference
	//	child.GetComponent<RelationshipComponent>().Parent = Entity();
	//}

	//void Entity::RemoveParent()
	//{
	//	Entity currentParent = GetParent();
	//	if (currentParent) { currentParent.RemoveChild(*this); }
	//}

	//bool Entity::HasParent()
	//{
	//	return GetComponent<RelationshipComponent>().Parent != entt::null;
	//}

	bool Entity::HasChildren()
	{
		bool hasChildren = m_Registry->all_of<HierarchyComponent>(m_EntityHandle) && m_Registry->get<HierarchyComponent>(m_EntityHandle).FirstChild != entt::null;
		return hasChildren;
	}

	//int Entity::GetChildCount()
	//{
	//	return (int)GetComponent<RelationshipComponent>().Children.size();
	//}

	//Entity Entity::GetParent()
	//{
	//	auto parent = m_Registry->all_of<ParentComponent>(m_EntityHandle) ? m_Registry->get<ParentComponent>(m_EntityHandle).parent : entt::null;
	//	if(parent == entt::null) return Entity(); // Return an invalid entity
	//	return Entity(parent, m_Registry);
	//}

	//std::vector<Entity> Entity::GetChildren()
	//{
	//	std::vector<Entity> childrenEntities;
	//	if (m_Registry->all_of<ChildrenComponent>(m_EntityHandle)) {
	//		const auto& children = m_Registry->get<ChildrenComponent>(m_EntityHandle).children;
	//		childrenEntities.reserve(children.size());
	//		for (const auto& child : children) {
	//			childrenEntities.emplace_back(child, m_Registry);
	//		}
	//	}
	//	return childrenEntities;
	//}

	//bool Entity::IsDescendant(entt::entity parent, entt::entity potentialChild) {
	//	if (!m_Registry->valid(potentialChild)) return false;

	//	if (!m_Registry->all_of<ChildrenComponent>(parent))
	//		return false;

	//	for (auto child : m_Registry->get<ChildrenComponent>(parent).children) {
	//		if (child == potentialChild || IsDescendant(child, potentialChild))
	//			return true;
	//	}
	//	return false;
	//}

	//void Entity::ReorderChild(entt::entity child, size_t newIndex)
	//{
	//	if (parent == entt::null) {
	//		// Move in root entity list
	//		auto& roots = m_Scene->GetRootEntities();

	//		auto it = std::find(roots.begin(), roots.end(), child);
	//		if (it != roots.end())
	//			roots.erase(it);

	//		newIndex = std::min(newIndex, roots.size());
	//		roots.insert(roots.begin() + newIndex, child);

	//		// Clear any parent component
	//		if (registry.all_of<ParentComponent>(child))
	//			registry.remove<ParentComponent>(child);

	//		return;
	//	}


	//	if (!m_Registry->valid(m_EntityHandle) || !m_Registry->valid(child))
	//		return;

	//	// Only reorder if already a child of parent
	//	if (!m_Registry->all_of<ChildrenComponent>(m_EntityHandle))
	//		return;

	//	auto& children = m_Registry->get<ChildrenComponent>(m_EntityHandle).children;

	//	// Remove from current position
	//	auto it = std::find(children.begin(), children.end(), child);
	//	if (it != children.end())
	//		children.erase(it);

	//	// Clamp insert index
	//	newIndex = std::min(newIndex, children.size());

	//	// Insert at new position
	//	children.insert(children.begin() + newIndex, child);
	//}

	//Entity Entity::GetRoot()
	//{
	//	Entity parent = GetParent();
	//	if (!parent) return *this;
	//	while (parent.HasParent())
	//	{
	//		parent = parent.GetParent();
	//	}
	//	return parent;
	//}
}