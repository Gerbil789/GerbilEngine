#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include <glm/glm.hpp>

namespace Engine
{
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
	{
		//ENGINE_LOG_TRACE("Created entity with ID: {0}", entity);
	}

	Scene::Scene()
	{
		struct TransformComponent
		{
			glm::mat4 Transform = glm::mat4(1.0f);

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& transform) : Transform(transform) {}
		
			operator glm::mat4& () { return Transform; }
			operator const glm::mat4& () const { return Transform; }
		};

		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			auto& transform = view.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<TransformComponent>);
	}

	Scene::~Scene()
	{

	}
}