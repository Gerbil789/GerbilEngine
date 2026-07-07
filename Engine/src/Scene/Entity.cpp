#include "enginepch.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	Entity::Entity(uint32_t handle, Scene* scene) : m_Handle(handle), m_Scene(scene) {}

	void Entity::SetActive(bool active)
	{
		if (!IsValid()) return;

		if (active)
		{
			m_Scene->GetRegistry().remove<DisabledTag>(static_cast<entt::entity>(m_Handle));
		}
		else
		{
			m_Scene->GetRegistry().emplace<DisabledTag>(static_cast<entt::entity>(m_Handle));
		}
	}

	bool Entity::IsActive() const
	{
		if (!IsValid()) return false;
		return !m_Scene->GetRegistry().all_of<DisabledTag>(static_cast<entt::entity>(m_Handle));
	}


	void Entity::Destroy()
	{
		if (IsValid())
		{
			m_Scene->DestroyEntity(*this);
			m_Handle = 0xFFFFFFFF; // Invalidate this handle immediately!
			m_Scene = nullptr;
		}
	}

	template<typename T>
	T& Entity::AddComponent()
	{
		return m_Scene->GetRegistry().emplace<T>(static_cast<entt::entity>(m_Handle));
	}

	template<typename T>
	T& Entity::GetComponent()
	{
		return m_Scene->GetRegistry().get<T>(static_cast<entt::entity>(m_Handle));
	}

	template<typename T>
	bool Entity::HasComponent()
	{
		return m_Scene->GetRegistry().all_of<T>(static_cast<entt::entity>(m_Handle));
	}

	template<typename T>
	void Entity::RemoveComponent()
	{
		m_Scene->GetRegistry().remove<T>(static_cast<entt::entity>(m_Handle));
	}


#define INSTANTIATE_COMPONENT(ComponentType) \
        template ENGINE_API ComponentType& Entity::AddComponent<ComponentType>(); \
        template ENGINE_API ComponentType& Entity::GetComponent<ComponentType>(); \
        template ENGINE_API bool Entity::HasComponent<ComponentType>(); \
        template ENGINE_API void Entity::RemoveComponent<ComponentType>();

	//INSTANTIATE_COMPONENT(DisabledTag)

	INSTANTIATE_COMPONENT(IdentityComponent)

	INSTANTIATE_COMPONENT(NameComponent)

	INSTANTIATE_COMPONENT(TransformComponent)

	INSTANTIATE_COMPONENT(MeshComponent)

	INSTANTIATE_COMPONENT(ColliderComponent)

	INSTANTIATE_COMPONENT(CameraComponent)

	INSTANTIATE_COMPONENT(LightComponent)

	INSTANTIATE_COMPONENT(ScriptComponent)

}