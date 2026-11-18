#include "enginepch.h"
#include "engine/core/scene.h"
#include "engine/core/components.h"

namespace engine
{
	void Scene::Update()
	{
		for (const auto& system : systems)
		{
			system(*this);
		}
	}

	void Scene::Registersystem(std::function<void(Scene&)> system)
	{
		systems.push_back(std::move(system));
	}

	entt::entity Scene::CreateEntity(std::string_view name, entt::entity parent)
	{
		entt::entity entity = registry.create();

		registry.emplace<component::Identity>(entity).id = Uuid::Generate();
		registry.emplace<component::Name>(entity).name = name;
		registry.emplace<component::Transform>(entity);
		registry.emplace<component::WorldTransform>(entity);
		registry.emplace<component::Relationship>(entity);

		if (parent != entt::null)
		{
			AttachChild(parent, entity);
		}
		else
		{
			AttachRoot(entity);
		}

		return entity;
	}

	void Scene::AttachChild(entt::entity parent, entt::entity child)
	{
		UnlinkFromHierarchy(child);

		auto& childRel = registry.get<component::Relationship>(child);
		auto& parentRel = registry.get<component::Relationship>(parent);

		childRel.parent = parent;
		childRel.prevSibling = entt::null;
		childRel.nextSibling = entt::null;

		if (parentRel.firstChild == entt::null)
		{
			parentRel.firstChild = child;
		}
		else
		{
			// Traverse to the last child to append
			entt::entity current = parentRel.firstChild;
			while (registry.get<component::Relationship>(current).nextSibling != entt::null)
			{
				current = registry.get<component::Relationship>(current).nextSibling;
			}

			registry.get<component::Relationship>(current).nextSibling = child;
			childRel.prevSibling = current;
		}
	}

	void Scene::AttachRoot(entt::entity entity)
	{
		UnlinkFromHierarchy(entity);

		auto& rel = registry.get<component::Relationship>(entity);
		rel.parent = entt::null;
		rel.prevSibling = lastRoot;
		rel.nextSibling = entt::null;

		if (lastRoot != entt::null)
		{
			registry.get<component::Relationship>(lastRoot).nextSibling = entity;
		}
		else
		{
			firstRoot = entity;
		}
		lastRoot = entity;
	}

	void Scene::UnlinkFromHierarchy(entt::entity entity)
	{
		auto* rel = registry.try_get<component::Relationship>(entity);
		if (!rel) return;

		// 1. Fix horizontal sibling links
		if (rel->prevSibling != entt::null)
		{
			registry.get<component::Relationship>(rel->prevSibling).nextSibling = rel->nextSibling;
		}
		if (rel->nextSibling != entt::null)
		{
			registry.get<component::Relationship>(rel->nextSibling).prevSibling = rel->prevSibling;
		}

		// 2. Fix parent or root chain pointers
		if (rel->parent != entt::null)
		{
			if (auto* parentRel = registry.try_get<component::Relationship>(rel->parent))
			{
				if (parentRel->firstChild == entity)
				{
					parentRel->firstChild = rel->nextSibling;
				}
			}
		}
		else
		{
			if (firstRoot == entity) firstRoot = rel->nextSibling;
			if (lastRoot == entity) lastRoot = rel->prevSibling;
		}

		rel->parent = entt::null;
		rel->firstChild = entt::null;
		rel->prevSibling = entt::null;
		rel->nextSibling = entt::null;
	}

	void Scene::DestroyEntity(entt::entity entity)
	{
		if (entity == entt::null || registry.all_of<tag::Deleted>(entity)) return;

		// Unlink immediately so traversal stops seeing it
		UnlinkFromHierarchy(entity);

		// Mark for deferred cleanup
		registry.emplace_or_replace<tag::Deleted>(entity);

		// Recursively destroy children safely by caching nextSibling
		if (auto* rel = registry.try_get<component::Relationship>(entity))
		{
			entt::entity child = rel->firstChild;
			while (child != entt::null)
			{
				entt::entity nextChild = registry.get<component::Relationship>(child).nextSibling;
				DestroyEntity(child);
				child = nextChild;
			}
		}
	}

	void Scene::DeleteMarkedEntities()
	{
		auto view = registry.view<tag::Deleted>();
		registry.destroy(view.begin(), view.end());
	}
}