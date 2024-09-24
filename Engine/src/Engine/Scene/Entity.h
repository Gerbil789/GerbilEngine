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
		Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}
		Entity(const Entity& other) = default;
		UUID GetUUID() { return GetComponent<IDComponent>().ID; }

		void SetName(const std::string& name) { GetComponent<NameComponent>() = name; }
		const std::string& GetName() { return GetComponent<NameComponent>().Name; }

		void SetActive(bool active) { GetComponent<EnablingComponent>().Enabled = active; }
		bool IsActive() { return GetComponent<EnablingComponent>().Enabled; }

		void SetParent(Entity newParent);
		void AddChild(Entity child);
		void RemoveChild(Entity child);
		void RemoveParent();

		bool HasParent();
		bool HasChildren();
		int GetChildCount();

		Entity GetParent();
		std::vector<Entity> GetChildren();
		Entity GetRoot();



		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; } // entity id

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr; 

	public:
		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			if (HasComponent<T>() )
			{
				LOG_WARNING("Entity already has component!");
				return GetComponent<T>();
			}
			
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}
	};
}