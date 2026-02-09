#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/Core.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	class ENGINE_API Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, entt::registry* registry) : m_Handle(handle), m_Registry(registry) {}
		Entity(const Entity& other) = default;

		static Entity Null() { return {}; }

		template<typename T>
		bool HasComponent() const
		{
			return m_Registry && m_Registry->any_of<T>(m_Handle);
		}

		template<typename T>
		T& GetComponent()
		{
			ASSERT(HasComponent<T>(), std::format("Entity does not have component of type {}", typeid(T).name()).c_str());
			return m_Registry->get<T>(m_Handle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			ASSERT(HasComponent<T>(), std::format("Entity does not have component of type {}", typeid(T).name()).c_str());
			return m_Registry->get<T>(m_Handle);
		}

		template<typename T>
		T* TryGetComponent()
		{
			return m_Registry ? m_Registry->try_get<T>(m_Handle) : nullptr;
		}

		template<typename T>
		const T* TryGetComponent() const
		{
			return m_Registry ? m_Registry->try_get<T>(m_Handle) : nullptr;
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Registry->get_or_emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Registry->remove<T>(m_Handle);
		}

		void SetName(const std::string& name) { GetComponent<NameComponent>().name = name; }
		const std::string& GetName() const { return GetComponent<NameComponent>().name; }

		void SetActive(bool active) { GetComponent<IdentityComponent>().enabled = active; }
		bool IsActive() const { return GetComponent<IdentityComponent>().enabled; }

		Uuid GetUUID() { return GetComponent<IdentityComponent>().id; }
		const Uuid GetUUID() const { return GetComponent<IdentityComponent>().id; }

		void SetParent(Entity newParent, bool keepWorld = true);
		void RemoveParent(bool keepWorld = true);
		void AddChild(Entity child, bool keepWorld = true);
		void RemoveChild(Entity child);
		std::vector<Entity> GetChildren() const;
		Entity GetParent() const;
		void Destroy();

		operator bool() const { return m_Registry && m_Registry->valid(m_Handle); }
		entt::entity Handle() const { return m_Handle; }
		operator uint32_t() const { return (uint32_t)m_Handle; } // entity id

		bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Registry == other.m_Registry; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_Handle{ entt::null };
		entt::registry* m_Registry{ nullptr };
	};
}

namespace std
{
	template<>
	struct hash<Engine::Entity>
	{
		size_t operator()(const Engine::Entity& e) const noexcept
		{
			return std::hash<uint32_t>()((uint32_t)e);
		}
	};
}