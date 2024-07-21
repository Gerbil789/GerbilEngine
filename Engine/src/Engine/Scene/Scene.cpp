#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include <glm/glm.hpp>
#include "Engine/Scene/Components.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{

	}


	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);

		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);
		return entity;
	}


	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity.m_EntityHandle);
	}
}