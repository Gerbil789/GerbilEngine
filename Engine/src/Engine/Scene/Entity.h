#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"

namespace Engine
{
	class Entity
	{
	public:
		static Entity Null() { return Entity(entt::null, nullptr); }
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
			ASSERT(HasComponent<T>(), std::format("Entity does not have component of type {}", typeName).c_str());
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			const char* typeName = typeid(T).name();
			ASSERT(HasComponent<T>(), std::format("Entity does not have component of type {}", typeName).c_str());
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
		const std::string& GetName() const { return GetComponent<NameComponent>().name; }

		void SetActive(bool active) { GetComponent<IdentityComponent>().enabled = active; }
		bool IsActive() const { return GetComponent<IdentityComponent>().enabled; }

		UUID GetUUID() { return GetComponent<IdentityComponent>().id; }
		const UUID GetUUID() const { return GetComponent<IdentityComponent>().id; }

		void SetParent(Entity newParent, bool keepWorld = true);
		void RemoveParent(bool keepWorld = true);
		void AddChild(Entity child, bool keepWorld = true);
		void RemoveChild(Entity child);
		std::vector<Entity> GetChildren() const;
		Entity GetParent() const;
		void Destroy();

		operator bool() const { return m_EntityHandle != entt::null && m_Scene->m_Registry.valid(m_EntityHandle); }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; } // entity id

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

		const std::string GetInfo() const
		{
			if (!(*this) || !m_Scene)
				return "Entity[null]";

			std::ostringstream ss;
			ss << "Entity Info\n";
			ss << "-----------\n";
			ss << "Handle: " << (uint32_t)m_EntityHandle << "\n";

			if (HasComponent<IdentityComponent>())
			{
				const auto& idc = GetComponent<IdentityComponent>();
				ss << "UUID: " << idc.id << "\n";
				ss << "Active: " << (idc.enabled ? "true" : "false") << "\n";
			}

			if (HasComponent<NameComponent>())
			{
				ss << "Name: " << GetComponent<NameComponent>().name << "\n";
			}

			// Hierarchy
			if (HasComponent<TransformComponent>())
			{
				const auto& tc = GetComponent<TransformComponent>();
				if (tc.parent != entt::null)
				{
					Entity parent(tc.parent, m_Scene);
					ss << "Parent: "
						<< (parent.HasComponent<NameComponent>() ? parent.GetName() : std::to_string((uint32_t)tc.parent))
						<< "\n";
				}
				else
				{
					ss << "Parent: [none]\n";
				}
				const auto& children = GetChildren();

				ss << "Children: " << children.size() << "\n";
				for (auto child : children)
				{
					Entity c(child, m_Scene);
					ss << " - "
						<< (c.HasComponent<NameComponent>() ? c.GetName() : std::to_string((uint32_t)child))
						<< "\n";
				}
			}
			return ss.str();
		}

	private:
		entt::entity m_EntityHandle { entt::null };
		Scene* m_Scene = nullptr;

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