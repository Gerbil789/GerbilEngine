#include "enginepch.h"
#include "Entity.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	void Entity::SetParent(Entity newParent)
	{
		Entity currentParent = GetParent();															// Get the current parent
		if (currentParent) { currentParent.RemoveChild(*this); }									// Remove this entity from the current parent's children		
		GetComponent<RelationshipComponent>().Parent = newParent;									// Set the new parent
		if(newParent) newParent.GetComponent<RelationshipComponent>().Children.push_back(*this);	// Add this entity to the new parent's children
	}

	void Entity::AddChild(Entity child)
	{
		child.SetParent(*this);
	}

	void Entity::RemoveChild(Entity child)
	{
		if(!child) return; // Return if the child is invalid (null entity)
		auto& children = GetComponent<RelationshipComponent>().Children;
		auto it = std::remove(children.begin(), children.end(), child);
		if (it != children.end())
		{
			children.erase(it);
		}

		// Update the child's parent reference
		child.GetComponent<RelationshipComponent>().Parent = Entity();
	}

	void Entity::RemoveParent()
	{
		Entity currentParent = GetParent();
		if (currentParent) { currentParent.RemoveChild(*this); }
	}

	bool Entity::HasParent()
	{
		return GetComponent<RelationshipComponent>().Parent != entt::null;
	}

	bool Entity::HasChildren()
	{
		return GetComponent<RelationshipComponent>().Children.size() > 0;
	}

	int Entity::GetChildCount()
	{
		return (int)GetComponent<RelationshipComponent>().Children.size();
	}

	Entity Entity::GetParent()
	{
		auto parent = GetComponent<RelationshipComponent>().Parent;
		if(parent == entt::null) return Entity(); // Return an invalid entity
		Entity entity = Entity(parent, m_Scene);
		return entity;
	}

	std::vector<Entity> Entity::GetChildren()
	{
		std::vector<Entity> children;
		for (auto child : GetComponent<RelationshipComponent>().Children)
		{
			children.push_back(Entity(child, m_Scene));
		}
		return children;
	}

	Entity Entity::GetRoot()
	{
		Entity parent = GetParent();
		if (!parent) return *this;
		while (parent.HasParent())
		{
			parent = parent.GetParent();
		}
		return parent;
	}
}