#include "enginepch.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Asset/AssetManager.h"

namespace engine
{
	SceneAsset::~SceneAsset()
	{
		m_Registry.clear();
	}

	void SceneAsset::DestroyEntity(Entity entity)
	{
		if (!entity) return;

		Uuid uuid = m_Registry.get<IdentityComponent>(entity.GetHandle()).id;
		m_Registry.destroy(entity.GetHandle());
		m_EntityMap.erase(uuid);
	}

	Entity SceneAsset::GetEntity(Uuid entityId)
	{
		if (m_EntityMap.find(entityId) != m_EntityMap.end())
		{
			return Entity(m_EntityMap[entityId], this);
		}
		return Entity{};
	}

	const std::vector<entt::entity>& SceneAsset::GetRootEntities() const
	{
		return m_RootEntities;
	}

	void SceneAsset::InsertRootEntity(entt::entity entity, size_t index)
	{
		if (index > m_RootEntities.size())
		{
			index = m_RootEntities.size();
		}
		m_RootEntities.insert(m_RootEntities.begin() + index, entity);
	}

	void SceneAsset::RemoveRootEntity(entt::entity entity)
	{
		auto it = std::find(m_RootEntities.begin(), m_RootEntities.end(), entity);
		if (it != m_RootEntities.end())
		{
			m_RootEntities.erase(it);
		}
	}
}