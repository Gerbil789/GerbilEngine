#pragma once

#include "Engine/Asset/Handle.h"
#include "Engine/Math/BoundingVolumeHiearchy.h"
#include <entt/entity/registry.hpp>

namespace engine
{
	struct Scene
	{
		entt::registry registry;
		entt::entity firstRoot = entt::null;
		entt::entity lastRoot = entt::null;
		entt::entity primaryCameraEntity = entt::null;
		TextureHandle environmentTexture = texture::HDR;
		BoundingVolumeHiearchy bvh;
		std::vector<std::function<void(Scene&)>> systems;

		Scene() = default;

		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		void Update();

		void RegisterSystem(std::function<void(Scene&)> system);

		entt::entity CreateEntity(std::string_view name = "Entity", entt::entity parent = entt::null);

		void AttachChild(entt::entity parent, entt::entity child);
		void AttachRoot(entt::entity entity);
		void UnlinkFromHierarchy(entt::entity entity);
		void DestroyEntity(entt::entity entity);
		void DeleteMarkedEntities();


	};
}