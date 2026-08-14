#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include <entt/entity/registry.hpp>

namespace Engine
{
	class ENGINE_API Scene : public Asset
	{
	public:
		Scene() = default;
		~Scene();

		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;


		//TODO: auto insert into root if not parent specified
		template<typename... Components>
		Entity CreateEntity(std::string_view name = "Entity")
		{
			entt::entity entity = m_Registry.create();
			Uuid uuid = Uuid::Generate();

			m_Registry.emplace<IdentityComponent>(entity, uuid);
			m_Registry.emplace<NameComponent>(entity).name = name;
			m_Registry.emplace<HierarchyComponent>(entity);

			(m_Registry.emplace<Components>(entity), ...);

			m_EntityMap[uuid] = entity;
			return Entity(entity, this);
		}


		void DestroyEntity(Entity entity);
		Entity GetEntity(Uuid entityId);

		entt::registry& GetRegistry() { return m_Registry; }

		const std::vector<entt::entity>& GetRootEntities() const;
		void InsertRootEntity(entt::entity entity, size_t index);
		void RemoveRootEntity(entt::entity entity);

		entt::entity GetActiveCamera();

		Uuid GetEnvironmentTexture() const { return m_EnvironmentTextureId; }
		void SetEnvironmentTexture(Uuid textureId) { m_EnvironmentTextureId = textureId; }

	private:
		entt::registry m_Registry;
		std::unordered_map<Engine::Uuid, entt::entity> m_EntityMap;
		std::vector<entt::entity> m_RootEntities;

		Uuid m_EnvironmentTextureId;

	private:
		friend class SceneSerializer;
	};
}