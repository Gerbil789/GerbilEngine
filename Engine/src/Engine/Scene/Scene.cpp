#include "enginepch.h"
#include "Scene.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	Scene::~Scene()
	{
		m_Registry.clear();
	}

	Entity Scene::CreateEntity(const std::string& name, const glm::vec3& position)
	{
		return CreateEntity(UUID(), name, position);
	}

	Entity Scene::CreateEntity(UUID uuid, const std::string& name, const glm::vec3& position)
	{
		entt::entity handle = m_Registry.create();
		Entity entity = { handle, this };
		entity.AddComponent<IdentityComponent>(uuid);
		entity.AddComponent<NameComponent>(name);
		entity.AddComponent<TransformComponent>(position);
		return entity;
	}

	Entity Scene::GetEntity(UUID uuid)
	{
		auto view = m_Registry.view<IdentityComponent>();
		for (auto entity : view)
		{
			if (view.get<IdentityComponent>(entity).ID == uuid)
				return Entity{ entity, this };
		}
		return Entity();
	}
}