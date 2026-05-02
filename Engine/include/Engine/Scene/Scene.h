#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Scene/Components.h"
#include <entt.hpp>

namespace Engine
{
	class ENGINE_API Scene : public Asset
	{
	public:
		Scene() = default;
		~Scene();

		Scene(Scene&& other) noexcept;
		Scene& operator=(Scene&& other) noexcept;

		//static Scene Copy(Scene& other);

		entt::entity CreateEntity(const std::string& name = "new entity");

		entt::entity GetEntity(Uuid uuid);
		entt::entity GetActiveCamera();
		void SetActiveCamera(entt::entity entity);
		entt::registry& GetRegistry() { return m_Registry; }
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
		entt::registry m_Registry;
		entt::entity m_ActiveCamera{ entt::null };
	};
}