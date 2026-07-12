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
		m_Registry.emplace<WorldTransformComponent>(entity);
		m_Registry.emplace<HierarchyComponent>(entity);

		m_EntityMap[uuid] = entity;
		return Entity(entity, this);
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (!entity) return;

		Uuid uuid = m_Registry.get<IdentityComponent>(entity.GetHandle()).id;
		m_Registry.destroy(entity.GetHandle());
		m_EntityMap.erase(uuid);
	}

	Entity Scene::GetEntity(Uuid entityId)
	{
		if (m_EntityMap.find(entityId) != m_EntityMap.end())
		{
			return Entity(m_EntityMap[entityId], this);
		}
		return Entity{};
	}

	const std::vector<entt::entity>& Scene::GetRootEntities() const
	{
		return m_RootEntities;
	}

	void Scene::InsertRootEntity(entt::entity entity, size_t index)
	{
		if (index > m_RootEntities.size())
		{
			index = m_RootEntities.size();
		}
		m_RootEntities.insert(m_RootEntities.begin() + index, entity);
	}

	void Scene::RemoveRootEntity(entt::entity entity)
	{
		auto it = std::find(m_RootEntities.begin(), m_RootEntities.end(), entity);
		if (it != m_RootEntities.end())
		{
			m_RootEntities.erase(it);
		}
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