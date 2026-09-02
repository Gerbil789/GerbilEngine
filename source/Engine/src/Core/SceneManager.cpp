#include "enginepch.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Core/Log.h"

namespace engine::SceneManager
{
	static Scene m_ActiveScene;

	void SetActiveScene(Scene scene)
	{
		m_ActiveScene = scene;

		//SceneAsset& sceneAsset = AssetManager::GetAsset<SceneAsset>(m_ActiveScene);
		//entt::registry& registry = sceneAsset.GetRegistry();
		//auto view = registry.view<CameraComponent, EditorTag>();

		//if (view.front() == entt::null)
		//{
		//	Entity entity = sceneAsset.CreateEntity<TransformComponent, WorldTransformComponent, CameraComponent, EditorTag, CameraProjectionDirty, CameraViewDirty, TransformDirty>("Editor Camera");
		//	entity.GetComponent<TransformComponent>().position = glm::vec3{ 0.0f, 0.0f, -20.0f };
		//	entity.GetComponent<CameraComponent>().background = CameraComponent::Background::Skybox;

		//	sceneAsset.InsertRootEntity(entity.GetHandle(), sceneAsset.GetRootEntities().size());
		//	sceneAsset.SetActiveCamera(entity.GetHandle());
		//}
		//else
		//{
		//	sceneAsset.SetActiveCamera(view.front());
		//}

		engine::EventBus::Publish(SceneChangedEvent{ m_ActiveScene });

		LOG_INFO("Active scene set to {}", m_ActiveScene.id);
	}

	Scene GetActiveScene()
	{
		return m_ActiveScene;
	}
}
