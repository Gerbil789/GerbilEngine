#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/TransformSystem.h"

namespace Engine
{
	class Camera;

	class ENGINE_API Scene : public Asset
	{
	public:
		Scene() = default;
		~Scene();

		Scene(const Scene& other);
		Scene& operator=(const Scene& other);

		Scene(Scene&& other) noexcept;
		Scene& operator=(Scene&& other) noexcept;

		entt::entity CreateEntity(const std::string& name = "new entity");
		entt::entity CreateEntity(const std::string& name, Uuid entityId);

		entt::entity GetEntity(Uuid uuid);
		entt::registry& GetRegistry() { return m_Registry; }

		Camera* GetActiveCamera() const;

		template<typename... Components>
		std::vector<entt::entity> GetEntities(bool includeDisabled = false)
		{
			std::vector<entt::entity> entities;

			auto view = m_Registry.view<IdentityComponent, Components...>();
			for (auto entity : view)
			{
				if(!includeDisabled && !m_Registry.get<IdentityComponent>(entity).enabled)
				{
					continue;
				}
				entities.push_back(entity);
			}
			return entities;
		}

	private:
		template<typename T>
		void CopyComponentIfExists(entt::entity dst, entt::entity src, const entt::registry& srcRegistry)
		{
			if (srcRegistry.any_of<T>(src))
			{
				m_Registry.emplace_or_replace<T>(dst, srcRegistry.get<T>(src));
			}
		}
	private:
		entt::registry m_Registry;
		std::unordered_map<Engine::Uuid, entt::entity> m_EntityMap; // for faster lookup by UUID
	};
}