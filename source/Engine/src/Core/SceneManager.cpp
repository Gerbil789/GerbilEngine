#include "enginepch.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Core/Log.h"

namespace Engine::SceneManager
{
	static Uuid m_ActiveScene;

	void SetActiveScene(Uuid id)
	{
		m_ActiveScene = id;

		Scene& scene = AssetManager::GetAsset<Scene>(m_ActiveScene);
		entt::registry& registry = scene.GetRegistry();
		auto view = registry.view<CameraComponent, EditorTag>();

		if (view.front() == entt::null)
		{
			Entity entity = scene.CreateEntity<TransformComponent, WorldTransformComponent, CameraComponent, EditorTag, PrimaryCameraTag, CameraProjectionDirty, CameraViewDirty, TransformDirty>("Editor Camera");
			entity.GetComponent<TransformComponent>().position = glm::vec3{ 0.0f, 0.0f, -20.0f };
			entity.GetComponent<CameraComponent>().background = CameraComponent::Background::Skybox;

			scene.InsertRootEntity(entity.GetHandle(), scene.GetRootEntities().size());
			scene.SetActiveCamera(entity.GetHandle());
		}
		else
		{
			scene.SetActiveCamera(view.front());
		}

		Engine::EventBus::Publish(SceneChangedEvent{ m_ActiveScene });

		LOG_INFO("Active scene set to {}", m_ActiveScene);
	}

	Uuid GetActiveScene()
	{
		return m_ActiveScene;
	}
}
