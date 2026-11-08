#pragma once

#include "Engine/Core/Components.h"
#include "Engine/Math/BoundingVolumeHiearchy.h"
#include <entt/entity/registry.hpp>

namespace engine
{
	template<typename T>
	concept UpdateSystem = requires(entt::registry & registry) { { T::Update(registry) }; };

	struct Scene
	{
		entt::registry registry;
		std::unordered_map<engine::Uuid, entt::entity> entityMap;
		std::vector<entt::entity> rootEntities;
		entt::entity primaryCameraEntity = entt::null;
		TextureHandle environmentTexture = texture::HDR;
		BoundingVolumeHiearchy bvh;
		std::vector<std::function<void()>> systems;

		Scene() = default;

		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		template<typename... Components>
		entt::entity CreateEntity(std::string_view name = "Entity", entt::entity parent = entt::null)
		{
			entt::entity entity = registry.create();
			Uuid uuid = Uuid::Generate();

			registry.emplace<component::Identity>(entity, uuid);
			registry.emplace<component::Name>(entity).name = name;
			auto& hc = registry.emplace<component::Hierarchy>(entity);

			if(parent == entt::null)
			{
 				InsertRootEntity(entity, rootEntities.size());
			}
			else
			{
				hc.parent = parent;
				registry.get<component::Hierarchy>(parent).children.push_back(entity);
			}

			(registry.emplace<Components>(entity), ...);

			entityMap[uuid] = entity;
			return entity;
		}

		void DestroyEntity(entt::entity entity);
		void DeleteMarkedEntities();

		entt::entity GetEntity(Uuid entityId) const;

		const component::CameraComponent* GetPrimaryCamera() const;

		void InsertRootEntity(entt::entity entity, size_t index);
		void RemoveRootEntity(entt::entity entity);

		template<typename System> requires UpdateSystem<System>
		void RegisterSystem()
		{
			systems.push_back([this]() {System::Update(registry);});
		}

		const std::vector<std::function<void()>>& GetSystems() const;
	};
}