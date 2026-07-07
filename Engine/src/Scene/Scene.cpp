#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	Scene::~Scene()
	{
		m_Registry.clear();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		entt::entity entity = m_Registry.create();
		Uuid uuid = Uuid::Generate();
		m_Registry.emplace<IdentityComponent>(entity, uuid);
		m_Registry.emplace<NameComponent>(entity, name);
		m_Registry.emplace<TransformComponent>(entity);

		m_EntityMap[uuid] = entity;
		return Entity(static_cast<uint32_t>(entity), this);
	}

	Entity Scene::CreateEntity(const std::string& name, Uuid entityId)
	{
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<IdentityComponent>(entity, entityId);
		m_Registry.emplace<NameComponent>(entity, name);
		m_Registry.emplace<TransformComponent>(entity);
		
		m_EntityMap[entityId] = entity;
		return Entity(static_cast<uint32_t>(entity), this);
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (entity.IsValid())
		{
			Uuid uuid = m_Registry.get<IdentityComponent>(static_cast<entt::entity>(entity.GetHandle())).id;
			m_Registry.destroy(static_cast<entt::entity>(entity.GetHandle()));
			m_EntityMap.erase(uuid);
		}
	}

	Entity Scene::GetEntity(Uuid uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end()) 
		{
			return Entity(static_cast<uint32_t>(m_EntityMap[uuid]), this);
		}
		return Entity(0xFFFFFFFF, this);
	}

	Camera* Scene::GetActiveCamera() const
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			if (m_Registry.get<CameraComponent>(entity).primary)
			{
				return m_Registry.get<CameraComponent>(entity).camera;
			}
		}
		return nullptr;
	}
}