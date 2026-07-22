#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Scene/Entity.h"
#include <entt/entity/registry.hpp>

namespace Engine
{
	class Camera;

	class ENGINE_API Scene : public Asset
	{
	public:
		Scene() = default;
		~Scene();

		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		Entity CreateEntity(const std::string& name = "entity");
		void DestroyEntity(Entity entity);
		Entity GetEntity(Uuid entityId);

		entt::registry& GetRegistry() { return m_Registry; }

		const std::vector<entt::entity>& GetRootEntities() const;
		void InsertRootEntity(entt::entity entity, size_t index);
		void RemoveRootEntity(entt::entity entity);

		Camera* GetActiveCamera() const; //TODO: improve camera management

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