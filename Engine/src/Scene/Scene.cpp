#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	Scene::~Scene()
	{
		m_Registry.clear();
	}

	Scene::Scene(Scene&& other) noexcept = default;
	Scene& Scene::operator=(Scene&& other) noexcept = default;

	Scene::Scene(const Scene& other)
	{
		std::unordered_map<entt::entity, entt::entity> entityMap;

		auto view = other.m_Registry.view<const IdentityComponent>();
		for (auto srcEntity : view)
		{
			Uuid uuid = other.m_Registry.get<IdentityComponent>(srcEntity).id;
			auto name = other.m_Registry.get<NameComponent>(srcEntity).name;

			entt::entity dstEntity = CreateEntity(name);

			m_Registry.replace<IdentityComponent>(dstEntity, uuid);

			entityMap[srcEntity] = dstEntity;

			CopyComponentIfExists<IdentityComponent>(dstEntity, srcEntity, other.m_Registry);
			CopyComponentIfExists<NameComponent>(dstEntity, srcEntity, other.m_Registry);
			CopyComponentIfExists<TransformComponent>(dstEntity, srcEntity, other.m_Registry);
			CopyComponentIfExists<CameraComponent>(dstEntity, srcEntity, other.m_Registry);
			CopyComponentIfExists<MeshComponent>(dstEntity, srcEntity, other.m_Registry);
			CopyComponentIfExists<ColliderComponent>(dstEntity, srcEntity, other.m_Registry);
			CopyComponentIfExists<LightComponent>(dstEntity, srcEntity, other.m_Registry);
			CopyComponentIfExists<ScriptComponent>(dstEntity, srcEntity, other.m_Registry);
		}

		//this->id = other.id;
		//this->editor_name = other.editor_name + "_copy";
	}

	Scene& Scene::operator=(const Scene& other)
	{
		if (this != &other)
		{
			m_Registry.clear();
			Uuid oldId = id;
			*this = Scene(other);
			id = oldId;
		}
		return *this;
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