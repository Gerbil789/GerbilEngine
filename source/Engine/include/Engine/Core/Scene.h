#pragma once

#include "Engine/Core/Components.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Math/BoundingVolumeHiearchy.h"

namespace engine
{
	template<typename T>
	concept UpdateSystem = requires(entt::registry & registry) { { T::Update(registry) }; };

	struct Scene
	{
		Scene() = default;
		~Scene();

		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		entt::registry registry;
		std::unordered_map<engine::Uuid, entt::entity> entityMap;
		std::vector<entt::entity> rootEntities;


		entt::entity m_PrimaryCameraEntity = entt::null;

		TextureHandle environmentTexture = texture::HDR;
		BoundingVolumeHiearchy bvh;

		std::vector<std::function<void()>> systems;



		//TODO: auto insert into root if not parent specified
		template<typename... Components>
		entt::entity CreateEntity(std::string_view name = "Entity")
		{
			entt::entity entity = registry.create();
			Uuid uuid = Uuid::Generate();

			registry.emplace<IdentityComponent>(entity, uuid);
			registry.emplace<NameComponent>(entity).name = name;
			registry.emplace<HierarchyComponent>(entity);

			(registry.emplace<Components>(entity), ...);

			entityMap[uuid] = entity;
			return entity;
		}

		void DestroyEntity(entt::entity entity);
		entt::entity GetEntity(Uuid entityId) const;

		CameraComponent* GetPrimaryCamera()
		{
			if (registry.valid(m_PrimaryCameraEntity))
			{
				return registry.try_get<CameraComponent>(m_PrimaryCameraEntity);
			}

			return nullptr;
		}

		void SetPrimaryCamera(entt::entity entity)
		{
			m_PrimaryCameraEntity = entity;
		}

		entt::entity GetPrimaryCameraEntity() const { return m_PrimaryCameraEntity; }

		const std::vector<entt::entity>& GetRootEntities() const;
		void InsertRootEntity(entt::entity entity, size_t index);
		void RemoveRootEntity(entt::entity entity);

		TextureHandle GetEnvironmentTexture() const;
		void SetEnvironmentTexture(TextureHandle texture);

		template<typename System> requires UpdateSystem<System>
		void RegisterSystem()
		{
			systems.push_back([this]() {System::Update(registry);});
		}

		const std::vector<std::function<void()>>& GetSystems() const;
	};
}