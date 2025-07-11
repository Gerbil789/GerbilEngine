#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Components.h"
#include "entt.hpp"

namespace Engine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, entt::registry* registry) : m_EntityHandle(handle), m_Registry(registry) {}
		Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Registry(&scene->m_Registry) {}
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent()
		{
			return m_Registry->any_of<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Registry->get<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Registry->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Registry->remove<T>(m_EntityHandle);
		}

		void SetName(const std::string& name) { GetComponent<NameComponent>() = name; }
		const std::string& GetName() { return GetComponent<NameComponent>().Name; }

		void SetActive(bool active) { GetComponent<IdentityComponent>().Enabled = active; }
		bool IsActive() { return GetComponent<IdentityComponent>().Enabled; }

		UUID GetUUID() { return GetComponent<IdentityComponent>().ID; }

		//void SetParent(Entity newParent);
		//void AddChild(Entity child);
		//void RemoveChild(Entity child);
		//void RemoveParent();

		//bool HasParent();
		bool HasChildren();
		//int GetChildCount();

		Entity GetParent();
		std::vector<Entity> GetChildren();
		//Entity GetRoot();

		//bool IsDescendant(entt::entity parent, entt::entity potentialChild); // Check if this entity is a descendant of the given entity
		//void ReorderChild(entt::entity child, size_t newIndex);

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; } // entity id

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Registry == other.m_Registry; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle { entt::null };
		entt::registry* m_Registry = nullptr;

	};
}