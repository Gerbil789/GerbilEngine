#include "enginepch.h"
#include "Engine/Core/Runtime.h"


#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Script/Script.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/Event.h"

#include <Windows.h>

namespace Engine
{
	namespace
	{
		Engine::EventToken m_Token;
	}

	void Runtime::LoadScripts(const std::filesystem::path& dllPath)
	{
		if (!std::filesystem::exists(dllPath))
		{
			throw std::runtime_error("DLL path does not exist: " + dllPath.string());
		}

		HMODULE gameModule = LoadLibraryA(dllPath.string().c_str());
		if (!gameModule)
		{
			throw std::runtime_error("Failed to load DLL scripts");
		}

		using GameRegisterScriptsFn = int(*)(Engine::ScriptRegistry&);
		auto Game_Register_Fn = (GameRegisterScriptsFn)GetProcAddress(gameModule, "RegisterScripts");

		if (!Game_Register_Fn)
		{
			throw std::runtime_error("Failed to load RegisterScripts function from scripts dll");
		}

		Game_Register_Fn(Engine::g_ScriptRegistry);

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

		//set active camera

		auto cameraEntities = scene.GetEntities<Engine::CameraComponent>();
		if (!cameraEntities.empty())
		{
			scene.SetActiveCamera(cameraEntities[0]);
		}

		for (Engine::Entity& ent : scene.GetEntities<Engine::ScriptComponent>())
		{
			auto& sc = ent.Get<Engine::ScriptComponent>();
			if (sc.instance)
			{
				sc.instance->Self = ent;
				sc.instance->OnStart();
			}
		}

		m_Token = Engine::EventBus::Get().SubscribeToAll([](Engine::Event& e)
			{
				auto& scene = Engine::SceneManager::GetActiveScene();
				for (Engine::Entity& ent : scene.GetEntities<Engine::ScriptComponent>())
				{
					auto& sc = ent.Get<Engine::ScriptComponent>();
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
		for (auto& ent : scene.GetEntities<Engine::ScriptComponent>())
		{
			auto& sc = ent.Get<Engine::ScriptComponent>();
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
		for (auto& ent : scene.GetEntities<Engine::ScriptComponent>())
		{
			auto& scriptComp = ent.Get<Engine::ScriptComponent>();
			if (scriptComp.instance)
			{
				scriptComp.instance->OnUpdate();
			}
		}

		// update camera & audio listener
		for (auto& ent : scene.GetEntities<Engine::CameraComponent>())
		{
			Engine::Camera* cam = ent.Get<Engine::CameraComponent>().camera;
			const auto& pos = ent.Get<Engine::TransformComponent>().position;
			const auto& forward = cam->GetForward();
			const auto& up = cam->GetUp();
			Engine::Audio::SetListener(pos.x, pos.y, pos.z, forward.x, forward.y, forward.z, up.x, up.y, up.z);
			cam->SetPosition(pos);

			const auto& rot = ent.Get<Engine::TransformComponent>().rotation;
			cam->SetRotation(rot);
		}
	}
}
