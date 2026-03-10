#include "enginepch.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Engine
{
	void Entity::Destroy()
	{
		if (m_Registry && m_Registry->valid(m_Handle))
		{
			m_Registry->destroy(m_Handle);
		}

		m_Handle = entt::null;
		m_Registry = nullptr;
	}

	void Entity::SetName(const std::string& name)
	{
		Get<NameComponent>().name = name;
	}

	const std::string& Entity::GetName() const
	{
		return Get<NameComponent>().name;
	}

	void Entity::SetActive(bool active)
	{
		Get<IdentityComponent>().enabled = active;
	}

	bool Entity::IsActive() const
	{
		return Get<IdentityComponent>().enabled;
	}

	Uuid Entity::GetUUID()
	{
		return Get<IdentityComponent>().id;
	}

	const Uuid Entity::GetUUID() const
	{
		return Get<IdentityComponent>().id;
	}


	void Detach(Entity child)
	{
		auto& transform = child.Get<TransformComponent>();

		if (!transform.parent) return;

		auto& parentTransform = transform.parent.Get<TransformComponent>();

		// If this was the first child, update parent's pointer
		if (parentTransform.firstChild == child)
		{
			parentTransform.firstChild = transform.nextSibling;
		}

		// Fix sibling links
		if (transform.prevSibling)
			transform.prevSibling.Get<TransformComponent>().nextSibling = transform.nextSibling;
		if (transform.nextSibling)
			transform.nextSibling.Get<TransformComponent>().prevSibling = transform.prevSibling;

		// Clear child links
		transform.parent = Entity::Null();
		transform.prevSibling = Entity::Null();
		transform.nextSibling = Entity::Null();
	}

	// Internal: Attach child under parent at the front of the list
	void Attach(Entity parent, Entity child)
	{
		auto& parentTransform = parent.Get<TransformComponent>();
		auto& childTransform = child.Get<TransformComponent>();

		childTransform.parent = parent;
		childTransform.prevSibling = Entity::Null();
		childTransform.nextSibling = parentTransform.firstChild;

		if (parentTransform.firstChild)
		{
			auto& first = parentTransform.firstChild.Get<TransformComponent>();
			first.prevSibling = child;
		}

		parentTransform.firstChild = child;
	}


	bool IsDescendant(Entity ancestor, Entity descendant)
	{
		if (!ancestor || !descendant) return false;

		auto tc = descendant.Get<TransformComponent>();
		while (tc.parent)
		{
			if (tc.parent == ancestor) return true;
			tc = tc.parent.Get<TransformComponent>();
		}
		return false;
	}

	void Entity::SetParent(Entity newParent, bool keepWorld)
	{
		if (IsDescendant(*this, newParent))
		{
			return; // prevent cycles
		}

		Detach(*this);
		if (newParent)
		{
			Attach(newParent, *this);
		}

		if (keepWorld)
		{
			glm::mat4 worldMatrix = Get<TransformComponent>().GetWorldMatrix();

			// Recalculate local transform so world stays same
			auto& childTransform = Get<TransformComponent>();
			glm::mat4 parentWorld = glm::mat4(1.0f);
			if (childTransform.parent)
				parentWorld = childTransform.parent.Get<TransformComponent>().GetWorldMatrix();

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

	void Entity::RemoveParent(bool keepWorld)
	{
		SetParent(Entity::Null(), keepWorld);
	}

	void Entity::AddChild(Entity child, bool keepWorld)
	{
		child.SetParent(*this, keepWorld);
	}

	void Entity::RemoveChild(Entity child)
	{
		auto& childTransform = child.Get<TransformComponent>();
		if (childTransform.parent == *this)
		{
			Detach(child);
		}
	}

	Entity Entity::GetParent() const
	{
		return this->Get<TransformComponent>().parent;
	}

	std::vector<Entity> Entity::GetChildren() const
	{
		std::vector<Entity> result;
		auto& transform = this->Get<TransformComponent>();

		Entity child = transform.firstChild;
		while (child)
		{
		  result.push_back(child);
			child = child.Get<TransformComponent>().nextSibling;
		}

		return result;
	}
}