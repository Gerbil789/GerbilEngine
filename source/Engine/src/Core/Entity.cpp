#include "enginepch.h"
#include "Engine/Core/Entity.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"

namespace Engine
{
	Entity::Entity(entt::entity handle, SceneAsset* scene) : m_Handle(handle), m_Scene(scene) {}

	void Entity::SetActive(bool active)
	{
		if (active)
		{
			m_Scene->GetRegistry().remove<DisabledTag>(m_Handle);
		}
		else
		{
			m_Scene->GetRegistry().emplace_or_replace<DisabledTag>(m_Handle);
		}
	}

	bool Entity::IsActive() const
	{
		return !m_Scene->GetRegistry().all_of<DisabledTag>(m_Handle);
	}

	void Entity::Destroy()
	{
		m_Scene->DestroyEntity(*this);
		m_Handle = entt::null;
		m_Scene = nullptr;
	}

	template<typename T, typename... Args>
	T& Entity::AddComponent(Args&&... args)
	{
		return m_Scene->GetRegistry().emplace<T>(m_Handle, std::forward<Args>(args)...);
	}

	template<typename T>
	void Entity::AddTag()
	{
		m_Scene->GetRegistry().emplace_or_replace<T>(m_Handle);
	}

	template<typename T>
	bool Entity::HasTag()
	{
		return m_Scene->GetRegistry().all_of<T>(m_Handle);
	}

	template<typename T>
	void Entity::RemoveTag()
	{
		m_Scene->GetRegistry().remove<T>(m_Handle);
	}

	template<typename T>
	T& Entity::GetOrAddComponent()
	{
		return m_Scene->GetRegistry().get_or_emplace<T>(m_Handle);
	}

	template<typename T>
	T& Entity::GetComponent()
	{
		return m_Scene->GetRegistry().get<T>(m_Handle);
	}

	template<typename T>
	T* Entity::TryGetComponent()
	{
		return m_Scene->GetRegistry().try_get<T>(m_Handle);
	}

	template<typename T>
	bool Entity::HasComponent()
	{
		return m_Scene->GetRegistry().all_of<T>(m_Handle);
	}

	template<typename T>
	void Entity::RemoveComponent()
	{
		m_Scene->GetRegistry().remove<T>(m_Handle);
	}


#define INSTANTIATE_COMPONENT(ComponentType) \
        template ComponentType& Entity::AddComponent<ComponentType>(); \
        template ComponentType& Entity::GetComponent<ComponentType>(); \
        template bool Entity::HasComponent<ComponentType>(); \
        template void Entity::RemoveComponent<ComponentType>(); \
				template ComponentType& Entity::GetOrAddComponent<ComponentType>(); \
				template ComponentType* Entity::TryGetComponent<ComponentType>();


	template void Entity::AddTag<DisabledTag>();
	template void Entity::AddTag<TransformDirty>();
	template void Entity::AddTag<PrimaryCameraTag>();
	template void Entity::AddTag<CameraProjectionDirty>();
	template void Entity::AddTag<CameraViewDirty>();

	template bool Entity::HasTag<PrimaryCameraTag>();


	template void Entity::RemoveTag<PrimaryCameraTag>();

	INSTANTIATE_COMPONENT(IdentityComponent)

	INSTANTIATE_COMPONENT(NameComponent)

	INSTANTIATE_COMPONENT(TransformComponent)

	INSTANTIATE_COMPONENT(WorldTransformComponent)

	INSTANTIATE_COMPONENT(HierarchyComponent)

	INSTANTIATE_COMPONENT(MeshComponent)

	INSTANTIATE_COMPONENT(ColliderComponent)

	INSTANTIATE_COMPONENT(CameraComponent)

	INSTANTIATE_COMPONENT(LightComponent)

	INSTANTIATE_COMPONENT(ScriptComponent)

	// --- UI COMPONENTS HERE ---

	template void Entity::AddTag<UI::LayoutDirtyTag>();

	INSTANTIATE_COMPONENT(UI::RectTransform)

	INSTANTIATE_COMPONENT(UI::Canvas)

	INSTANTIATE_COMPONENT(UI::Image)

	INSTANTIATE_COMPONENT(UI::Text)
}