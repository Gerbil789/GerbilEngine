#include "enginepch.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Components.h"
#include "Engine/Script/Script.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/Event.h"

#include "Engine/Scene/CameraSystem.h"

#ifdef ENGINE_PLATFORM_WINDOWS
#include <Windows.h>
#elif defined(ENGINE_PLATFORM_LINUX)
#include <dlfcn.h> // POSIX dynamic loading (dlopen, dlsym)
#endif

#ifndef ENGINE_SHARED_EXPORT
extern "C" void RegisterScripts();
#endif

namespace Engine
{
	void Runtime::LoadScripts([[maybe_unused]] const std::filesystem::path& dllPath)
	{
#ifdef ENGINE_SHARED_EXPORT
		if (!std::filesystem::exists(dllPath))
		{
			throw std::runtime_error("Script library path does not exist: " + dllPath.string());
		}

		using GameRegisterScriptsFn = void(*)();
		GameRegisterScriptsFn Game_Register_Fn = nullptr;

		HMODULE gameModule = LoadLibraryA(dllPath.string().c_str());
		if (!gameModule) throw std::runtime_error("Failed to load Windows DLL scripts");

		Game_Register_Fn = reinterpret_cast<GameRegisterScriptsFn>(reinterpret_cast<void*>(GetProcAddress(gameModule, "RegisterScripts")));

		if (!Game_Register_Fn)
		{
			throw std::runtime_error("Failed to load RegisterScripts function from scripts library");
		}

		Game_Register_Fn();
#else
		LOG_INFO("Static build detected. Ignoring dynamic path and calling linked RegisterScripts directly.");
		RegisterScripts();
#endif

		//const auto& scripts = Engine::ScriptRegistry::GetScripts();

		//LOG_INFO("Total Registered Scripts: {}", scripts.size());
		//for (const auto& [id, script] : scripts)
		//{
		//	LOG_INFO("  {}", script.name);
		//}
	}

	void Runtime::Start()
	{
		Engine::Scene& scene = Engine::AssetManager::GetAsset<Scene>(Engine::SceneManager::GetActiveScene());
		entt::registry& registry = scene.GetRegistry();

	/*	for(entt::entity cameraEntity : registry.view<Engine::CameraComponent>())
		{
			auto& cameraComp = registry.get<Engine::CameraComponent>(cameraEntity);
			if (cameraComp.primary && cameraComp.camera)
			{
				const auto& pos = registry.get<Engine::TransformComponent>(cameraEntity).position;
				const auto& forward = cameraComp.camera->GetForward();
				const auto& up = cameraComp.camera->GetUp();
				Engine::Audio::SetListener(pos.x, pos.y, pos.z, forward.x, forward.y, forward.z, up.x, up.y, up.z);
				scene.SetActiveCamera(cameraComp.camera);
				break;
			}
		}*/


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
		auto& scene = Engine::AssetManager::GetAsset<Scene>(Engine::SceneManager::GetActiveScene());
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

		auto& scene = Engine::AssetManager::GetAsset<Scene>(Engine::SceneManager::GetActiveScene());
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
		{
			entt::entity cameraEntity = scene.GetActiveCamera();

			if(cameraEntity != entt::null)
			{
				auto& tc = registry.get<TransformComponent>(cameraEntity);

				const auto& forward = CameraSystem::GetForward(tc);
				const auto& up = CameraSystem::GetUp(tc);
				Engine::Audio::SetListener(tc.position.x, tc.position.y, tc.position.z, forward.x, forward.y, forward.z, up.x, up.y, up.z);
			}
		}
		
	}
}
