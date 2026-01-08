#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Scene/Components.h"
#include <entt.hpp>

namespace Engine
{
	class Entity;

	class Scene : public Asset
	{
	public:
		Scene() = default;
		~Scene();

		Entity CreateEntity(const std::string& name = "new entity", const glm::vec3& position = glm::vec3{ 0.0f, 0.0f, 0.0f });
		Entity CreateEntity(UUID uuid, const std::string& name = "new entity", const glm::vec3& position = glm::vec3{ 0.0f, 0.0f, 0.0f });

		Entity GetEntity(UUID uuid);
		Entity GetActiveCamera();
		void SetActiveCamera(Entity entity);


		entt::registry& Registry() { return m_Registry; }

		template<typename... Components>
		std::vector<Entity> GetEntities(bool includeDisabled = false)
		{
			std::vector<Entity> entities;

			auto view = m_Registry.view<IdentityComponent, Components...>();
			for (auto entity : view)
			{
				if(!includeDisabled && !m_Registry.get<IdentityComponent>(entity).enabled)
				{
					continue;
				}
				entities.push_back(Entity{ entity, &m_Registry });
			}
			return entities;
		}

	private:
		entt::registry m_Registry;
		entt::entity m_ActiveCamera{ entt::null };
	};

}