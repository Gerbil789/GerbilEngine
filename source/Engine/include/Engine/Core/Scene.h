#pragma once

#include "Engine/Core/Components.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include <entt/entity/registry.hpp>

namespace engine
{
	template<typename T>
	concept UpdateSystem = requires(entt::registry & registry) { { T::Update(registry) }; };

	struct SceneAsset
	{
		SceneAsset() = default;
		~SceneAsset();

		SceneAsset(SceneAsset&&) noexcept = default;
		SceneAsset& operator=(SceneAsset&&) noexcept = default;

		SceneAsset(const SceneAsset&) = delete;
		SceneAsset& operator=(const SceneAsset&) = delete;

		entt::registry registry;
		std::unordered_map<engine::Uuid, entt::entity> entityMap;
		std::vector<entt::entity> rootEntities;
		Texture2D environmentTexture;

		std::vector<std::function<void()>> systems;
	};

	struct Scene : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		SceneAsset& Get() const;

		//TODO: auto insert into root if not parent specified
		template<typename... Components>
		entt::entity CreateEntity(std::string_view name = "Entity")
		{
			SceneAsset& data = Get();
			entt::entity entity = data.registry.create();
			Uuid uuid = Uuid::Generate();

			data.registry.emplace<IdentityComponent>(entity, uuid);
			data.registry.emplace<NameComponent>(entity).name = name;
			data.registry.emplace<HierarchyComponent>(entity);

			(data.registry.emplace<Components>(entity), ...);

			data.entityMap[uuid] = entity;
			return entity;
		}

		void DestroyEntity(entt::entity entity);
		entt::entity GetEntity(Uuid entityId) const;


		entt::registry& GetRegistry();
		const entt::registry& GetRegistry() const;

		CameraComponent* GetPrimaryCamera() const;

		const std::vector<entt::entity>& GetRootEntities() const;
		void InsertRootEntity(entt::entity entity, size_t index);
		void RemoveRootEntity(entt::entity entity);

		Texture2D GetEnvironmentTexture() const;
		void SetEnvironmentTexture(Texture2D texture);

		template<typename System> requires UpdateSystem<System>
		void RegisterSystem()
		{
			Get().systems.push_back([this]() {System::Update(Get().registry);});
		}

		const std::vector<std::function<void()>>& GetSystems() const;

		static const Scene Default;
	};

	inline constexpr Scene Scene::Default{ 0 };
}