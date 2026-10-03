#include "enginepch.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Asset/AssetManager.h"

namespace engine
{
	SceneAsset::~SceneAsset()
	{
		registry.clear();
	}

	SceneAsset& Scene::Get() const
	{
		return AssetManager::GetAsset(*this);
	}

	void Scene::DestroyEntity(entt::entity entity)
	{
		if (entity == entt::null) return;
		SceneAsset& data = Get();

		Uuid id = data.registry.get<IdentityComponent>(entity).id;
		data.registry.destroy(entity);
		data.entityMap.erase(id);
	}

	entt::entity Scene::GetEntity(Uuid entityId) const
	{
		const auto& map = Get().entityMap;
		if (auto it = map.find(entityId); it != map.end())
		{
			return it->second;
		}
		return entt::null;
	}



	entt::registry& Scene::GetRegistry()
	{
		return Get().registry;
	}

	const entt::registry& Scene::GetRegistry() const
	{
		return Get().registry;
	}

CameraComponent* Scene::GetPrimaryCamera() const
{
    entt::registry& registry = Get().registry;
    auto view = registry.view<engine::CameraComponent, engine::PrimaryCameraTag>();
    
    if (view.front() == entt::null)
    {
			LOG_WARNING("No active camera in the scene");
      return nullptr;
    }
    
    return &registry.get<CameraComponent>(view.front());
}

	const std::vector<entt::entity>& Scene::GetRootEntities() const
	{
		return Get().rootEntities;
	}

	void Scene::InsertRootEntity(entt::entity entity, size_t index)
	{
		auto& rootEntities = Get().rootEntities;
		if (index > rootEntities.size())
		{
			index = rootEntities.size();
		}
		rootEntities.insert(rootEntities.begin() + index, entity);
	}

	void Scene::RemoveRootEntity(entt::entity entity)
	{
		auto& rootEntities = Get().rootEntities;
		auto it = std::find(rootEntities.begin(), rootEntities.end(), entity);
		if (it != rootEntities.end())
		{
			rootEntities.erase(it);
		}
	}


	Texture2D Scene::GetEnvironmentTexture() const
	{
		return Get().environmentTexture;
	}
	void Scene::SetEnvironmentTexture(Texture2D texture)
	{
		Get().environmentTexture = texture;
	}

	const std::vector<std::function<void()>>& Scene::GetSystems() const
	{
		return Get().systems;
	}
}