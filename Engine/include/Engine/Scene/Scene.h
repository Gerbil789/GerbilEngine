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

		Entity CreateEntity(const std::string& name = "new entity");
		Entity CreateEntity(const std::string& name, Uuid entityId);

		void DestroyEntity(Entity entity);

		Entity GetEntity(Uuid uuid);

		entt::registry& GetRegistry() { return m_Registry; }

		Camera* GetActiveCamera() const;

		Uuid GetEnvironmentTexture() const { return m_EnvironmentTextureId; }
		void SetEnvironmentTexture(Uuid textureId) { m_EnvironmentTextureId = textureId; }

	private:
		entt::registry m_Registry;
		std::unordered_map<Engine::Uuid, entt::entity> m_EntityMap; // for faster lookup by UUID

		Uuid m_EnvironmentTextureId;
	};
}