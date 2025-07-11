#include "enginepch.h"
#include "Scene.h"
#include "Engine/Scene/ScriptableEntity.h"
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

	Scene::Scene(const std::filesystem::path& path) : Asset(path)
	{
		//m_Registry.storage<IDComponent>();
		//m_Registry.storage<TransformComponent>();
		//m_Registry.storage<MeshComponent>();
	}

	Scene::~Scene()
	{
		//auto view = m_Registry.view<IDComponent>();
		//LOG_INFO("Destroying scene with {0} entities", view.size());
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

	template<typename Component>
	static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = srcRegistry.view<Component>();
		for (auto srcEntity : view)
		{
			UUID uuid = srcRegistry.get<IdentityComponent>(srcEntity).ID;
			ASSERT(enttMap.find(uuid) != enttMap.end(), "Entity not found in map!");
			entt::entity dstEnttId = enttMap.at(uuid);

			auto& srcComponent = srcRegistry.get<Component>(srcEntity);
			dstRegistry.emplace_or_replace<Component>(dstEnttId, srcComponent);

		}
	}

	//template<typename Component>
	//static void CopyComponentIfExists(Entity src, Entity dst)
	//{
	//	if(src.HasComponent<Component>())
	//	{
	//		auto& srcComponent = src.GetComponent<Component>();
	//		dst.AddOrReplaceComponent<Component>(srcComponent);
	//	}
	//	
	//}

	Ref<Scene> Scene::Copy(const Ref<Scene>& other)
	{
		Ref<Scene> newScene = CreateRef<Scene>(other->GetFilePath());

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		// Copy entities
		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;

		auto IdView = srcSceneRegistry.view<IdentityComponent>();

		for (auto entity : IdView)
		{
			UUID uuid = IdView.get<IdentityComponent>(entity).ID;
			const auto& name = srcSceneRegistry.get<NameComponent>(entity).Name;
			Entity newEntity = newScene->CreateEntity(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;

		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		//CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}


	//void Scene::OnUpdate(Timestep ts)
	//{
	//	//get scene camera  and its transform from scene

	//	auto view = m_Registry.view<TransformComponent, CameraComponent>();
	//	glm::mat4 transform = glm::mat4(1.0f);
	//	Camera* camera = nullptr;

	//	for (auto entity : view)
	//	{
	//		auto [transformComponent, cameraComponent] = view.get<TransformComponent, CameraComponent>(entity);
	//		if (cameraComponent.Main)
	//		{
	//			camera = &cameraComponent.Camera;
	//			transform = transformComponent.GetTransform();
	//			break;
	//		}
	//	}

	//	if (!camera) { return; }

	//	switch (m_SceneState)
	//	{
	//	case SceneState::Editor:
	//		OnUpdateEditor(ts, *camera, transform);
	//		break;
	//	case SceneState::Runtime:
	//		//OnUpdateRuntime(ts);
	//		break;
	//	}
	//}

	//void Scene::OnUpdate(Timestep ts, EditorCamera& camera)
	//{
	//	switch (m_SceneState)
	//	{
	//	case SceneState::Editor:
	//		OnUpdateEditor(ts, camera);
	//		break;
	//	case SceneState::Runtime:
	//		OnUpdateRuntime(ts);
	//		break;
	//	}
	//}


	//void Scene::OnPlay()
	//{
	//	m_IsPlaying = true;
	//}

	//void Scene::OnStop()
	//{
	//	m_IsPlaying = false;
	//}

	//void Scene::OnPause()
	//{
	//	m_IsPaused = true;
	//}

	//void Scene::OnResume()
	//{
	//	m_IsPaused = false;
	//}



	//void Scene::OnUpdateRuntime(Timestep ts)
	//{
	//	//scripts
	//	//{
	//	//	m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
	//	//	{
	//	//		//TODO: move this to scene::onplay
	//	//		if (!nsc.Instance)
	//	//		{
	//	//			nsc.Instance = nsc.InstantiateScript();
	//	//			nsc.Instance->m_Entity = Entity{ entity, this };
	//	//			nsc.Instance->OnCreate();
	//	//			
	//	//		}
	//	//		nsc.Instance->OnUpdate(ts);
	//	//	});
	//	//}


	//void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	//{
	//	Renderer::BeginFrame(camera);

	//	Renderer::RenderScene(this,camera);

	//	Renderer::EndFrame();
	//}

	//void Scene::OnUpdateEditor(Timestep ts, Camera& camera, const glm::mat4& transform)
	//{

	//}


	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();

		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				Engine::Camera& camera = cameraComponent.Camera;
				camera.SetViewportSize(width, height);
			}
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
}