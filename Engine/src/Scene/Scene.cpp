#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	Scene::~Scene()
	{
		m_Registry.clear();
	}

	Scene* Scene::Copy(Scene* other)
	{
		Scene* newScene = new Scene();
		auto& srcRegistry = other->m_Registry;
		auto& dstRegistry = newScene->m_Registry;

		std::unordered_map<Uuid, entt::entity> uuidToEntityMap;

		auto view = srcRegistry.view<IdentityComponent>();
		for (auto srcEntity : view)
		{
			const auto& id = srcRegistry.get<IdentityComponent>(srcEntity).id;
			const auto& enabled = srcRegistry.get<IdentityComponent>(srcEntity).enabled;
			const auto& name = srcRegistry.get<NameComponent>(srcEntity).name;

			Entity newEntity = newScene->CreateEntity(name);
			newEntity.GetComponent<IdentityComponent>().id = id;
			newEntity.GetComponent<IdentityComponent>().enabled = enabled;
			const auto& transform = srcRegistry.get<TransformComponent>(srcEntity);
			newEntity.GetComponent<TransformComponent>() = transform;

			uuidToEntityMap[id] = newEntity.Handle();
		}

		auto copyComponent = [&](auto typeTag)
			{
				using Component = decltype(typeTag);

				auto componentView = srcRegistry.view<Component>();
				for (auto srcEntity : componentView)
				{
					const auto& id = srcRegistry.get<IdentityComponent>(srcEntity);
					entt::entity dstEntity = uuidToEntityMap[id.id];

					const auto& srcComponent = componentView.get<Component>(srcEntity);
					dstRegistry.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			};

		copyComponent(MeshComponent{});
		copyComponent(ScriptComponent{});
		copyComponent(LightComponent{});
		copyComponent(CameraComponent{});


		if (other->m_ActiveCamera != entt::null)
		{
			const auto& id = srcRegistry.get<IdentityComponent>(other->m_ActiveCamera);
			newScene->m_ActiveCamera = uuidToEntityMap[id.id];
		}

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		entt::entity handle = m_Registry.create();
		m_Registry.emplace<IdentityComponent>(handle, Uuid(), true);
		m_Registry.emplace<NameComponent>(handle, name);
		m_Registry.emplace<TransformComponent>(handle);
		return Entity{ handle, &m_Registry };
	}

	Entity Scene::GetActiveCamera()
	{
		if (m_ActiveCamera == entt::null)
		{
			return {}; // Return null entity wrapper
		}

		return Entity{ m_ActiveCamera, &m_Registry };
	}

	void Scene::SetActiveCamera(Entity entity)
	{
		if (!entity.HasComponent<CameraComponent>())
		{
			LOG_WARNING("Setting active camera to entity without CameraComponent!");
			return;
		}
		m_ActiveCamera = entity.Handle();
	}

	Entity Scene::GetEntity(Uuid uuid)
	{
		auto view = m_Registry.view<IdentityComponent>();
		for (auto entity : view)
		{
			if (view.get<IdentityComponent>(entity).id == uuid)
			{
				return Entity{ entity, &m_Registry };
			}
		}
		return Entity();
	}
}