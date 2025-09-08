#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			const char* typeName = typeid(T).name();
			ASSERT(HasComponent<T>(), fmt::format("Entity does not have component of type {}", typeName).c_str());
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			const char* typeName = typeid(T).name();
			ASSERT(HasComponent<T>(), fmt::format("Entity does not have component of type {}", typeName).c_str());
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		T* TryGetComponent()
		{
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename T>
		const T* TryGetComponent() const
		{
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
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
		bool HasChildren()
		{
			//bool hasChildren = m_Scene->m_Registry->all_of<HierarchyComponent>(m_EntityHandle) && m_Scene->m_Registry->get<HierarchyComponent>(m_EntityHandle).FirstChild != entt::null;
			//return hasChildren;

			return false;
		}
		//int GetChildCount();

		//Entity GetParent();
		//std::vector<Entity> GetChildren();
		//Entity GetRoot();

		//bool IsDescendant(entt::entity parent, entt::entity potentialChild); // Check if this entity is a descendant of the given entity
		//void ReorderChild(entt::entity child, size_t newIndex);

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; } // entity id

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle { entt::null };
		Scene* m_Scene = nullptr;

	};
}