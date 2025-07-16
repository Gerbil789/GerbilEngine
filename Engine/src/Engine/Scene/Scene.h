#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Core/Asset.h"
#include "Engine/Scene/Components.h"
#include <entt.hpp>

namespace Engine 
{
	class Entity; // Forward declaration

	class SceneFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data = std::any()) override;
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data = std::any()) override;
	};

	class Scene : public Asset
	{
	public:
		Scene(const std::filesystem::path& path);
		~Scene();
		void RefreshRootEntities(); //TODO: move to SceneController?

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntity(UUID uuid, const std::string& name = "");
		void DestroyEntity(Entity entity);

		template<typename... Components>
		std::vector<Entity> GetEntities()
		{
			std::vector<Entity> entities;

			auto view = m_Registry.view<IdentityComponent, Components...>();
			for (auto entity : view)
			{
				entities.push_back(Entity{ entity, &m_Registry });
			}
			return entities;
		}

		std::vector<entt::entity>& GetRootEntities() { return m_RootEntities; }
		const std::vector<entt::entity>& GetRootEntities() const { return m_RootEntities; }

		void AddRootEntity(entt::entity entity);
		void RemoveRootEntity(entt::entity entity);
		void ReorderRootEntity(entt::entity entity, size_t newIndex);

		void SetParent(entt::entity child, entt::entity newParent);
		bool IsDescendant(entt::entity parent, entt::entity child);

		void CopyFrom(const Scene& other);

		entt::registry m_Registry;

	private:
		std::vector<entt::entity> m_RootEntities;

		friend class SceneController;
		friend class Entity;
		friend class SceneHierarchyPanel;
	};

}