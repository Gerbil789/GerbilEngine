#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	Scene::~Scene()
	{
		m_Registry.clear();
	}

	entt::entity Scene::CreateEntity(const std::string& name)
	{
		entt::entity entity = m_Registry.create();
		Uuid uuid = Uuid::Generate();
		m_Registry.emplace<IdentityComponent>(entity, uuid);
		m_Registry.emplace<NameComponent>(entity, name);
		m_Registry.emplace<TransformComponent>(entity);

		m_EntityMap[uuid] = entity;
		return entity;
	}

	entt::entity Scene::CreateEntity(const std::string& name, Uuid entityId)
	{
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<IdentityComponent>(entity, entityId);
		m_Registry.emplace<NameComponent>(entity, name);
		m_Registry.emplace<TransformComponent>(entity);
		
		m_EntityMap[entityId] = entity;
		return entity;
	}

	entt::entity Scene::GetEntity(Uuid uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end()) 
		{
			return m_EntityMap.at(uuid);
		}
		return entt::null;
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