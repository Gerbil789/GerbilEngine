#include "enginepch.h"
#include "Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/Serializer.h"

namespace Engine
{
	Ref<Asset> SceneFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		Ref<Scene> scene = CreateRef<Scene>(path);
		if (!Serializer::Deserialize(scene))
		{
			LOG_ERROR("Failed to load scene from file {0}", path.string());
		}
		scene->RefreshRootEntities();

		return scene;
	}

	Ref<Asset> SceneFactory::Create(const std::filesystem::path& path, const std::any& data)
	{
		return CreateRef<Scene>(path);
	}

	Scene::Scene(const std::filesystem::path& path) : Asset(path) {}

	Scene::~Scene()
	{
		m_Registry.clear();
	}

	void Scene::RefreshRootEntities()
	{
		m_RootEntities.clear();

		auto view = m_Registry.view<HierarchyComponent>();
		for (auto entity : view)
		{
			const auto& hc = view.get<HierarchyComponent>(entity);
			if (hc.Parent == entt::null)
				m_RootEntities.push_back(entity);
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntity(UUID(), name);
	}

	Entity Scene::CreateEntity(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), &m_Registry };
		entity.AddComponent<IdentityComponent>(uuid);
		entity.AddComponent<NameComponent>(name);
		entity.AddComponent<HierarchyComponent>();
		entity.AddComponent<TransformComponent>();

		m_RootEntities.push_back(entity);

		return entity;
	}


	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		auto view = m_Registry.view<IdentityComponent>();
		for (auto entity : view)
		{
			if (view.get<IdentityComponent>(entity).ID == uuid)
				return Entity{ entity, &m_Registry };
		}
		return Entity();
	}

	void Scene::AddRootEntity(entt::entity entity)
	{
		auto& hc = m_Registry.get_or_emplace<HierarchyComponent>(entity);
		hc.Parent = entt::null;
		m_RootEntities.push_back(entity);
	}

	void Scene::RemoveRootEntity(entt::entity entity)
	{
		m_RootEntities.erase(std::remove(m_RootEntities.begin(), m_RootEntities.end(), entity), m_RootEntities.end());
	}

	void Scene::ReorderRootEntity(entt::entity entity, size_t newIndex)
	{
		auto& roots = m_RootEntities;
		auto it = std::find(roots.begin(), roots.end(), entity);
		if (it == roots.end())
			return;

		roots.erase(it);
		if (newIndex >= roots.size())
			roots.push_back(entity);
		else
			roots.insert(roots.begin() + newIndex, entity);
	}

	void Scene::SetParent(entt::entity child, entt::entity newParent)
	{
		// Check for cycles, if the child is already the new parent or a descendant of it, do nothing
		if (child == newParent || IsDescendant(child, newParent))
			return;

		auto& hc = m_Registry.get_or_emplace<HierarchyComponent>(child);

		// Unlink from current parent or root
		if (hc.Parent != entt::null)
		{
			auto& oldParentHC = m_Registry.get<HierarchyComponent>(hc.Parent);
			if (oldParentHC.FirstChild == child)
				oldParentHC.FirstChild = hc.NextSibling;

			if (hc.PrevSibling != entt::null)
				m_Registry.get<HierarchyComponent>(hc.PrevSibling).NextSibling = hc.NextSibling;
			if (hc.NextSibling != entt::null)
				m_Registry.get<HierarchyComponent>(hc.NextSibling).PrevSibling = hc.PrevSibling;
		}

		// Set new parent
		hc.Parent = newParent;
		hc.NextSibling = entt::null;
		hc.PrevSibling = entt::null;

		if (newParent != entt::null)
		{
			auto& parentHC = m_Registry.get_or_emplace<HierarchyComponent>(newParent);
			hc.NextSibling = parentHC.FirstChild;
			if (hc.NextSibling != entt::null)
				m_Registry.get<HierarchyComponent>(hc.NextSibling).PrevSibling = child;
			parentHC.FirstChild = child;
		}
	}

	bool Scene::IsDescendant(entt::entity parent, entt::entity child) 
	{
		if (parent == entt::null || child == entt::null)
			return false;

		const auto* hierarchy = m_Registry.try_get<HierarchyComponent>(child);
		while (hierarchy && hierarchy->Parent != entt::null)
		{
			if (hierarchy->Parent == parent)
				return true;

			hierarchy = m_Registry.try_get<HierarchyComponent>(hierarchy->Parent);
		}

		return false;
	}

	void Scene::CopyFrom(const Scene& other)
	{
		m_Registry.clear();
		m_RootEntities.clear();

		std::unordered_map<UUID, entt::entity> uuidToEntityMap;

		// Step 1: Create all entities with UUIDs
		auto view = other.m_Registry.view<IdentityComponent>();
		for (auto e : view)
		{
			const auto& id = view.get<IdentityComponent>(e).ID;
			Entity newEntity = CreateEntity(id);
			uuidToEntityMap[id] = (entt::entity)newEntity;
		}

		// Step 2: Copy all components (except IdentityComponent and HierarchyComponent)
		auto CopyComponent = [&](auto typeHolder)
			{
				using T = decltype(typeHolder);
				auto view = other.m_Registry.view<T>();
				for (auto e : view)
				{
					UUID uuid = other.m_Registry.get<IdentityComponent>(e).ID;
					entt::entity dst = uuidToEntityMap.at(uuid);
					const T& srcComponent = view.get<T>(e);
					m_Registry.emplace_or_replace<T>(dst, srcComponent);
				}
			};

		CopyComponent(TransformComponent{});
		CopyComponent(NameComponent{});
		CopyComponent(MeshComponent{});
		CopyComponent(SpriteComponent{});
		CopyComponent(CameraComponent{});
		CopyComponent(LightComponent{});


		// Step 3: Copy hierarchy
		{
			auto view = other.m_Registry.view<HierarchyComponent>();
			for (auto e : view)
			{
				UUID uuid = other.m_Registry.get<IdentityComponent>(e).ID;
				entt::entity dst = uuidToEntityMap.at(uuid);
				const auto& src = view.get<HierarchyComponent>(e);

				auto& dstHC = m_Registry.emplace_or_replace<HierarchyComponent>(dst);
				dstHC.Parent = src.Parent != entt::null ? uuidToEntityMap.at(other.m_Registry.get<IdentityComponent>(src.Parent).ID) : entt::null;
				dstHC.FirstChild = src.FirstChild != entt::null ? uuidToEntityMap.at(other.m_Registry.get<IdentityComponent>(src.FirstChild).ID) : entt::null;
				dstHC.NextSibling = src.NextSibling != entt::null ? uuidToEntityMap.at(other.m_Registry.get<IdentityComponent>(src.NextSibling).ID) : entt::null;
				dstHC.PrevSibling = src.PrevSibling != entt::null ? uuidToEntityMap.at(other.m_Registry.get<IdentityComponent>(src.PrevSibling).ID) : entt::null;
			}
		}

		// Step 4: Copy root entity list
		for (auto e : other.m_RootEntities)
		{
			const UUID& uuid = other.m_Registry.get<IdentityComponent>(e).ID;
			entt::entity dst = uuidToEntityMap.at(uuid);
			m_RootEntities.push_back(dst);
		}
	}

}