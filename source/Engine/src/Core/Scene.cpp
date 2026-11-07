#include "enginepch.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Asset/AssetManager.h"

namespace engine
{
	Scene::~Scene()
	{
		registry.clear();
	}

	void Scene::DestroyEntity(entt::entity entity)
	{
		if (entity == entt::null) return;

		Uuid id = registry.get<component::Identity>(entity).id;
		registry.destroy(entity);
		entityMap.erase(id);
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

	const std::vector<entt::entity>& Scene::GetRootEntities() const
	{
		return rootEntities;
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

	TextureHandle Scene::GetEnvironmentTexture() const
	{
		return environmentTexture;
	}

	void Scene::SetEnvironmentTexture(TextureHandle texture)
	{
		environmentTexture = texture;
	}

	const std::vector<std::function<void()>>& Scene::GetSystems() const
	{
		return systems;
	}
}