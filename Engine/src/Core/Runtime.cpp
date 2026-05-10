#include "enginepch.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Components.h"
#include "Engine/Script/Script.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/Event.h"

#ifdef ENGINE_PLATFORM_WINDOWS
#include <Windows.h>
#elif defined(ENGINE_PLATFORM_LINUX)
#include <dlfcn.h> // POSIX dynamic loading (dlopen, dlsym)
#endif

#ifndef ENGINE_BUILD_SHARED
extern "C" void RegisterScripts(Engine::ScriptRegistry&);
#endif

namespace Engine
{
	namespace
	{
		Engine::EventToken m_Token;
	}

	void Runtime::LoadScripts([[maybe_unused]] const std::filesystem::path& dllPath)
	{
#ifdef ENGINE_BUILD_SHARED
		// ==========================================
		// DYNAMIC LINKING (Windows Editor / DLL mode)
		// ==========================================
		if (!std::filesystem::exists(dllPath))
		{
			throw std::runtime_error("Script library path does not exist: " + dllPath.string());
		}

		using GameRegisterScriptsFn = void(*)(Engine::ScriptRegistry&);
		GameRegisterScriptsFn Game_Register_Fn = nullptr;

#ifdef ENGINE_PLATFORM_WINDOWS
		HMODULE gameModule = LoadLibraryA(dllPath.string().c_str());
		if (!gameModule) throw std::runtime_error("Failed to load Windows DLL scripts");

		Game_Register_Fn = reinterpret_cast<GameRegisterScriptsFn>(reinterpret_cast<void*>(GetProcAddress(gameModule, "RegisterScripts")));

#elif defined(ENGINE_PLATFORM_LINUX)
		// We include this just in case you ever decide to make a Linux Editor!
		void* gameModule = dlopen(dllPath.string().c_str(), RTLD_NOW | RTLD_LOCAL);
		if (!gameModule) throw std::runtime_error(std::string("Failed to load Linux .so scripts: ") + dlerror());

		Game_Register_Fn = reinterpret_cast<GameRegisterScriptsFn>(reinterpret_cast<void*>(dlsym(gameModule, "RegisterScripts")));
#endif

		if (!Game_Register_Fn)
		{
			throw std::runtime_error("Failed to load RegisterScripts function from scripts library");
		}

		Game_Register_Fn(Engine::g_ScriptRegistry);

#else
		// ==========================================
		// STATIC LINKING (Final Linux Export mode)
		// ==========================================
		LOG_INFO("Static build detected. Ignoring dynamic path and calling linked RegisterScripts directly.");

		// Because the game is statically linked, RegisterScripts is already in memory!
		RegisterScripts(Engine::g_ScriptRegistry);
#endif

		auto scripts = Engine::g_ScriptRegistry.GetAllDescriptors();

		LOG_INFO("Total Registered Scripts: {}", scripts.size());
		for (const auto& script : scripts)
		{
			LOG_INFO("  {}", script->name);
		}
	}

	void Runtime::Start()
	{
		Engine::Scene& scene = Engine::SceneManager::GetActiveScene();

		auto cameraEntities = scene.GetEntities<Engine::CameraComponent>();
		if (!cameraEntities.empty())
		{
			scene.SetActiveCamera(cameraEntities[0]);
		}

		for (entt::entity entity : scene.GetEntities<Engine::ScriptComponent>())
		{
			auto& sc = scene.GetRegistry().get<Engine::ScriptComponent>(entity);
			if (sc.instance)
			{
				sc.instance->m_Entity = entity;
				sc.instance->m_Scene = &scene;
				sc.instance->OnStart();
			}
		}

		m_Token = Engine::EventBus::Get().SubscribeToAll([](Engine::Event& e)
			{
				auto& scene = Engine::SceneManager::GetActiveScene();
				for (entt::entity entity : scene.GetEntities<Engine::ScriptComponent>())
				{
					auto& sc = scene.GetRegistry().get<Engine::ScriptComponent>(entity);
					if (sc.instance)
					{
						sc.instance->OnEvent(e);
					}
				}
			});
	}

	void Runtime::Stop()
	{
		auto& scene = Engine::SceneManager::GetActiveScene();
		for (entt::entity entity : scene.GetEntities<Engine::ScriptComponent>())
		{
			auto& sc = scene.GetRegistry().get<Engine::ScriptComponent>(entity);
			if (sc.instance)
			{
				sc.instance->OnDestroy();
			}
		}

		Engine::EventBus::Get().Unsubscribe(m_Token);

		Engine::Audio::StopAll();
		Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal);
	}

	void Runtime::Update()
	{
		if (Engine::Input::IsKeyPressedOnce(Engine::KeyCode::Escape))
		{
			Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal);
		}

		auto& scene = Engine::SceneManager::GetActiveScene();

		// update scripts
		for (entt::entity entity : scene.GetEntities<Engine::ScriptComponent>())
		{
			auto& scriptComp = scene.GetRegistry().get<Engine::ScriptComponent>(entity);
			if (scriptComp.instance)
			{
				scriptComp.instance->OnUpdate();
			}
		}

		// update camera & audio listener
		for (entt::entity entity : scene.GetEntities<Engine::CameraComponent>())
		{
			auto& cameraComp = scene.GetRegistry().get<Engine::CameraComponent>(entity);
			Engine::Camera* cam = cameraComp.camera;
			const auto& pos = scene.GetRegistry().get<Engine::TransformComponent>(entity).position;
			const auto& forward = cam->GetForward();
			const auto& up = cam->GetUp();
			Engine::Audio::SetListener(pos.x, pos.y, pos.z, forward.x, forward.y, forward.z, up.x, up.y, up.z);
			cam->SetPosition(pos);

			const auto& rot = scene.GetRegistry().get<Engine::TransformComponent>(entity).rotation;
			cam->SetRotation(rot);
		}
	}
}
