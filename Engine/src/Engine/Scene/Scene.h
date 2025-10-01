#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Scene/Components.h"
#include <entt.hpp>

namespace Engine
{
	class Entity;

	class Scene : public Asset
	{
		friend class Entity;

	public:
		Scene() = default;
		~Scene();

		template<typename... Components>
		std::vector<Entity> GetEntities()
		{
			std::vector<Entity> entities;

			auto view = m_Registry.view<IdentityComponent, Components...>();
			for (auto entity : view)
			{
				entities.push_back(Entity{ entity, this });
			}
			return entities;
		}

		Entity GetEntity(UUID uuid);

		Entity CreateEntity(const std::string& name = "new entity", const glm::vec3& position = glm::vec3{ 0.0f, 0.0f, 0.0f });
		Entity CreateEntity(UUID uuid, const std::string& name = "new entity", const glm::vec3& position = glm::vec3{ 0.0f, 0.0f, 0.0f });

	private:
		entt::registry m_Registry;
	};

}