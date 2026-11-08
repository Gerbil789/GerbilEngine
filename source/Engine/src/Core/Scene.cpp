#include "enginepch.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Asset/AssetManager.h"

namespace engine
{
	void Scene::DestroyEntity(entt::entity entity)
	{
		if (entity == entt::null || registry.all_of<tag::Deleted>(entity))
			return;

		registry.emplace_or_replace<tag::Deleted>(entity);

		auto& hc = registry.get<component::Hierarchy>(entity);
		for (auto child : hc.children)
		{
			DestroyEntity(child);
		}
	}

	void Scene::DeleteMarkedEntities()
	{
		auto view = registry.view<tag::Deleted>();
		std::vector<entt::entity> entitiesToDelete(view.begin(), view.end());

		for (auto entity : entitiesToDelete)
		{
			auto& hc = registry.get<component::Hierarchy>(entity);

			if (hc.parent != entt::null)
			{
				// If it has a parent, remove this entity from the parent's children list
				if (registry.valid(hc.parent))
				{
					if (auto* parentHc = registry.try_get<component::Hierarchy>(hc.parent))
					{
						std::erase(parentHc->children, entity);
					}
				}
			}
			else
			{
				std::erase(rootEntities, entity);
			}

			auto& identity = registry.get<component::Identity>(entity);
			entityMap.erase(identity.id);
			registry.destroy(entity);
		}
	}

	entt::entity Scene::GetEntity(Uuid entityId) const
	{
		const auto& map = entityMap;
		if (auto it = map.find(entityId); it != map.end())
		{
			return it->second;
		}
		return entt::null;
	}

	const component::CameraComponent* Scene::GetPrimaryCamera() const
	{
		if (registry.valid(primaryCameraEntity))
		{
			return registry.try_get<component::CameraComponent>(primaryCameraEntity);
		}
		return nullptr;
	}

	void Scene::InsertRootEntity(entt::entity entity, size_t index)
	{
		if (index > rootEntities.size())
		{
			index = rootEntities.size();
		}
		rootEntities.insert(rootEntities.begin() + index, entity);
	}

	void Scene::RemoveRootEntity(entt::entity entity)
	{
		auto it = std::find(rootEntities.begin(), rootEntities.end(), entity);
		if (it != rootEntities.end())
		{
			rootEntities.erase(it);
		}
	}

	const std::vector<std::function<void()>>& Scene::GetSystems() const
	{
		return systems;
	}
}