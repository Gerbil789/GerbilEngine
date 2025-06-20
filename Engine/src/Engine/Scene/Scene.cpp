#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include <glm/glm.hpp>
#include "Engine/Scene/Components.h"
#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Renderer2D.h"
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
		return scene;
	}

	Ref<Asset> SceneFactory::Create(const std::filesystem::path& path, const std::any& data)
	{
		return CreateRef<Scene>(path);
	}

	Scene::~Scene()
	{
		//auto view = m_Registry.view<IDComponent>();
		//LOG_INFO("Destroying scene with {0} entities", view.size());
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
		Ref<Scene> newScene = CreateRef<Scene>(other->GetFilePath());
		
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
			auto [transformComponent, cameraComponent] = view.get<TransformComponent, CameraComponent>(entity);
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
		LOG_WARNING("OnNextFrame not implemented yet!");
	}



	void Scene::OnUpdateRuntime(Timestep ts)
	{
		//scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				//TODO: move this to scene::onplay
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
		Renderer::BeginScene(camera);
		auto group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent, EnablingComponent>);
		for (auto entity : group)
		{
			if (!group.get<EnablingComponent>(entity).Enabled) { return; }
			auto [transform, mesh] = group.get<TransformComponent, MeshRendererComponent>(entity);
			Renderer::DrawMesh(transform.GetTransform(), mesh.Mesh, mesh.Material, (int)entity);
		}
		Renderer::EndScene();


		//TODO: 2D renderer is somehow colliding with 3D renderer -> 3D does not work when it is enabled
		/*Renderer2D::BeginScene(camera);
		auto group2D = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent, EnablingComponent>);
		for (auto entity : group2D)
		{
			if (!group2D.get<EnablingComponent>(entity).Enabled) { return; }
			auto [transform, sprite] = group2D.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}
		Renderer2D::EndScene();*/
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

	void Scene::OnDestroy()
	{
		m_Registry.clear();
		m_EntityMap.clear();
		m_EntityOrder.clear();
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
		entity.AddComponent<RelationshipComponent>();
		m_EntityMap[uuid] = entity;
		m_EntityOrder.push_back(uuid);
		return entity;
	}


	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetComponent<IDComponent>().ID);
		m_EntityOrder.erase(std::remove(m_EntityOrder.begin(), m_EntityOrder.end(), entity.GetComponent<IDComponent>().ID), m_EntityOrder.end());
		m_Registry.destroy(entity);
	}


	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		auto it = m_EntityMap.find(uuid);
		if (it != m_EntityMap.end())
		{
			return Entity(it->second, this);
		}
		return Entity(); // Return invalid entity if not found
	}

	std::vector<Entity> Scene::GetLightEntities()
	{
		std::vector<Entity> lightEntities;

		auto view = m_Registry.view<LightComponent>();
		for(auto entity : view)
		{
			lightEntities.push_back(Entity{ entity, this });
		}
		return lightEntities;
	}

	std::vector<Entity> Scene::GetEntities()
	{
		std::vector<Entity> entities;

		auto view = m_Registry.view<IDComponent>();
		for(auto entity : view)
		{
			entities.push_back(Entity{ entity, this });
		}
		return entities;
	}

	std::vector<Entity> Scene::GetEntitiesOrdered()
	{
		std::vector<Entity> entities;

		for(auto uuid : m_EntityOrder)
		{
			auto entity = GetEntityByUUID(uuid);
			if(entity)
			{
				entities.push_back(entity);
			}
		}
		return entities;
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

	bool Scene::IsEntitySelected(Entity entity) const
	{
		return m_SelectedEntity == entity;
	}

	Entity Scene::GetSelectedEntity() 
	{
		return { m_SelectedEntity, this };
	}

	void Scene::ReorderEntity(Entity sourceEntity, Entity targetEntity)
	{
		auto sourceUUID = sourceEntity.GetUUID();
		auto targetUUID = targetEntity.GetUUID();

		// Find the current position of the source entity
		auto sourceIt = std::find(m_EntityOrder.begin(), m_EntityOrder.end(), sourceUUID);
		if (sourceIt == m_EntityOrder.end())
			return;

		// Find the position of the target entity
		auto targetIt = std::find(m_EntityOrder.begin(), m_EntityOrder.end(), targetUUID);
		if (targetIt == m_EntityOrder.end())
			return;

		// Remove the source entity from its current position
		m_EntityOrder.erase(sourceIt);

		// Insert the source entity before the target entity
		targetIt = std::find(m_EntityOrder.begin(), m_EntityOrder.end(), targetUUID); // Refind the target iterator after erasure
		m_EntityOrder.insert(targetIt, sourceUUID);
	}

	void Scene::SelectMaterial(const Ref<Material>& material)
	{
		m_SelectedMaterial = material;
	}

	
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) 
	{
		ASSERT(false, "OnComponentAdded not implemented for this component type");
	}

	template<>
	void Scene::OnComponentAdded<NameComponent>(Entity entity, NameComponent& component) {}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {}

	template<>
	void Scene::OnComponentAdded<EnablingComponent>(Entity entity, EnablingComponent& component) {}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component) {}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {}

	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component) {}

	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component) {}
}