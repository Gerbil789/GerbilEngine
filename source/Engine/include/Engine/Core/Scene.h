#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Core/Entity.h"
#include "Engine/Core/Components.h"
#include <entt/entity/registry.hpp>

namespace Engine
{
	class SceneAsset : public Asset
	{
	public:
		SceneAsset() = default;
		~SceneAsset();

		SceneAsset(SceneAsset&&) noexcept = default;
		SceneAsset& operator=(SceneAsset&&) noexcept = default;

		SceneAsset(const SceneAsset&) = delete;
		SceneAsset& operator=(const SceneAsset&) = delete;


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

		Texture2D GetEnvironmentTexture() const { return m_EnvironmentTexture; }
		void SetEnvironmentTexture(Texture2D texture) { m_EnvironmentTexture = texture; }

	private:
		entt::registry m_Registry;
		std::unordered_map<Engine::Uuid, entt::entity> m_EntityMap;
		std::vector<entt::entity> m_RootEntities;
		Texture2D m_EnvironmentTexture;

	private:
		friend class SceneSerializer;
	};
}