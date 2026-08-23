#include "enginepch.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Components.h"
#include "Engine/Script/Script.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/Event.h"
#include "Engine/System/CameraSystem.h"

extern "C" void RegisterScripts();

namespace Engine
{
	void Runtime::LoadScripts()
	{
		RegisterScripts();
	}

	void Runtime::Start()
	{
		Engine::SceneAsset& scene = Engine::AssetManager::GetAsset(Engine::SceneManager::GetActiveScene());
		entt::registry& registry = scene.GetRegistry();

		for (entt::entity entity : registry.view<Engine::ScriptComponent>())
		{
			auto& sc = registry.get<Engine::ScriptComponent>(entity);
			if (sc.instance)
			{
				sc.instance->m_Entity = Entity(entity, &scene);
				sc.instance->OnStart();
			}
		}
	}

	void Runtime::Stop()
	{
		auto& scene = Engine::AssetManager::GetAsset<SceneAsset>(Engine::SceneManager::GetActiveScene());
		entt::registry& registry = scene.GetRegistry();

		auto view = registry.view<ScriptComponent>(entt::exclude<DisabledTag>);

		for (auto&& [entity, sc] : view.each())
		{
			if (sc.instance)
			{
				sc.instance->OnDestroy();
			}
		}

		//Engine::EventBus::Unsubscribe();

		Engine::Audio::StopAll();
		Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal);
	}

	void Runtime::Update()
	{
		if (Engine::Input::IsKeyPressedOnce(Engine::Key::Escape))
		{
			Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal);
		}

		auto& scene = Engine::AssetManager::GetAsset<SceneAsset>(Engine::SceneManager::GetActiveScene());
		entt::registry& registry = scene.GetRegistry();

		// update scripts
		{
			auto view = registry.view<ScriptComponent>(entt::exclude<DisabledTag>);

			for (auto&& [entity, sc] : view.each())
			{
				if (sc.instance)
				{
					sc.instance->OnUpdate();
				}
			}
		}

		// update camera & audio listener
		/*{
			entt::entity cameraEntity = scene.GetActiveCamera();

			if(cameraEntity != entt::null)
			{
				auto& tc = registry.get<TransformComponent>(cameraEntity);

				const auto& forward = CameraSystem::GetForward(tc);
				const auto& up = CameraSystem::GetUp(tc);
				Engine::Audio::SetListener(tc.position.x, tc.position.y, tc.position.z, forward.x, forward.y, forward.z, up.x, up.y, up.z);
			}
		}*/
		
	}
}
