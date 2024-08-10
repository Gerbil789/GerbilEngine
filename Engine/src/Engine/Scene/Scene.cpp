#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include <glm/glm.hpp>
#include "Engine/Scene/Components.h"
#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{
		//log entity count
		auto view = m_Registry.view<IDComponent>();
		ENGINE_LOG_INFO("Destroying scene with {0} entities", view.size());
	}

	template<typename Component>
	static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = srcRegistry.view<Component>();
		for(auto srcEntity : view)
		{
			UUID uuid = srcRegistry.get<IDComponent>(srcEntity).ID;
			ASSERT(enttMap.find(uuid) != enttMap.end(), "Entity not found in map!");
			entt::entity dstEnttId = enttMap.at(uuid);

			auto& srcComponent = srcRegistry.get<Component>(srcEntity);
			dstRegistry.emplace_or_replace<Component>(dstEnttId, srcComponent);

		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity src, Entity dst)
	{
		if(src.HasComponent<Component>())
		{
			auto& srcComponent = src.GetComponent<Component>();
			dst.AddOrReplaceComponent<Component>(srcComponent);
		}
		
	}

	Ref<Scene> Scene::Copy(const Ref<Scene>& other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		// Copy entities
		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;

		auto IdView = srcSceneRegistry.view<IDComponent>();

		for(auto entity : IdView)
		{
			UUID uuid = IdView.get<IDComponent>(entity).ID;
			const auto& name = srcSceneRegistry.get<NameComponent>(entity).Name;
			Entity newEntity = newScene->CreateEntity(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;

		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}


	void Scene::OnUpdate(Timestep ts)
	{
		//get scene camera  and its transform from scene
		
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		glm::mat4 transform = glm::mat4(1.0f);
		Camera* camera = nullptr;

		for (auto entity : view)
		{
			auto& [transformComponent, cameraComponent] = view.get<TransformComponent, CameraComponent>(entity);
			if(cameraComponent.Main)
			{
				camera = &cameraComponent.Camera;
				transform = transformComponent.GetTransform();
				break;
			}
		}

		if(!camera) { return; }

		switch (m_SceneState)
		{
		case SceneState::Editor:
			OnUpdateEditor(ts, *camera, transform);
			break;
		case SceneState::Runtime:
			//OnUpdateRuntime(ts);
			break;
		}
	}

	void Scene::OnUpdate(Timestep ts, EditorCamera& camera)
	{
		switch(m_SceneState)
		{
			case SceneState::Editor:
				OnUpdateEditor(ts, camera);
				break;
			case SceneState::Runtime:
				OnUpdateRuntime(ts);
				break;
		}
	}


	void Scene::OnPlay()
	{
		m_IsPlaying = true;
	}

	void Scene::OnStop()
	{
		m_IsPlaying = false;
	}

	void Scene::OnPause()
	{
		m_IsPaused = true;
	}

	void Scene::OnResume()
	{
		m_IsPaused = false;
	}

	void Scene::OnNextFrame()
	{
		ENGINE_LOG_WARNING("OnNextFrame not implemented yet!");
	}



	void Scene::OnUpdateRuntime(Timestep ts)
	{
		//scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				//todo: move this to scene::onplay
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
					
				}
				nsc.Instance->OnUpdate(ts);
			});
		}



		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if(camera.Main)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if(mainCamera == nullptr)
		{
			//ENGINE_LOG_WARNING("No main camera entity found!");
			return;
		}

		Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent, EnablingComponent>);
		for (auto entity : group)
		{
			if (!group.get<EnablingComponent>(entity).Enabled) { return; }
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateEditor(Timestep ts, Camera& camera, const glm::mat4& transform)
	{
		Renderer2D::BeginScene(camera, transform);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent, EnablingComponent>);
		for (auto entity : group)
		{
			if (!group.get<EnablingComponent>(entity).Enabled) { return; }
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}

		Renderer2D::EndScene();
	}


	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();

		for(auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if(!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntity(UUID(), name);
	}

	Entity Scene::CreateEntity(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<EnablingComponent>(true);
		entity.AddComponent<NameComponent>(name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}


	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	Entity Scene::GetEntityByName(const std::string& name)
	{
		auto view = m_Registry.view<NameComponent>();
		for (auto entity : view)
		{
			if(view.get<NameComponent>(entity).Name == name)
			{
				return Entity{ entity, this };
			}
		}
		return {};
		
	}

	

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());
		
		CopyComponentIfExists<TransformComponent>(entity, newEntity);
		CopyComponentIfExists<SpriteRendererComponent>(entity, newEntity);
		CopyComponentIfExists<CameraComponent>(entity, newEntity);
		CopyComponentIfExists<NativeScriptComponent>(entity, newEntity);

		SelectEntity(newEntity);
	}

	void Scene::CopyEntity(Entity entity)
	{
		m_CopiedEntityUUID = entity.GetUUID();
	}

	void Scene::PasteEntity()
	{
		if(m_CopiedEntityUUID == 0) { return; }

		auto view = m_Registry.view<IDComponent>();
		for(auto entity : view)
		{
			if(view.get<IDComponent>(entity).ID == m_CopiedEntityUUID)
			{
				DuplicateEntity(Entity{ entity, this });
				return;
			}
		}
	}

	void Scene::SelectEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void Scene::DeselectEntity()
	{
		m_SelectedEntity = entt::null;
	}

	bool Scene::IsEntitySelected(Entity entity)
	{
		return m_SelectedEntity == entity;
	}

	const Entity& Scene::GetSelectedEntity()
	{
		Entity entity = { m_SelectedEntity, this };
		return entity;
	}

	


	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) 
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<NameComponent>(Entity entity, NameComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<EnablingComponent>(Entity entity, EnablingComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}
}