#include "EditorRuntime.h"
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
#include "Editor/Windows/Viewport/ViewportWindow.h"
#include <Windows.h>

namespace Editor
{
	namespace 
	{
		bool m_IgnoredSceneChange = false;
		Engine::EventToken m_Token;
	}
	void EditorRuntime::Initialize()
	{
		Engine::SceneManager::RegisterOnSceneChanged([](Engine::Scene* scene) 
			{
				if(m_IgnoredSceneChange == false)
				{
					m_EditorScene = scene;
				}
			});
	}

	void EditorRuntime::LoadScripts(Engine::ScriptRegistry& registry, const std::filesystem::path& dllPath)
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

		Game_Register_Fn(registry);

		auto scripts = registry.GetAllDescriptors();

		LOG_INFO("Total Registered Scripts: {}", scripts.size());
		for (const auto& script : scripts)
		{
			LOG_INFO("  {}", script->name);
		}
	}

	void EditorRuntime::SetEditorState(EditorState newState)
	{
		m_State = newState;
		m_IgnoredSceneChange = true;
		if (m_State == EditorState::Play)
		{
			EditorRuntime::Start();
		}
		else
		{
			EditorRuntime::Stop();
		}

		m_IgnoredSceneChange = false;
	}

	void EditorRuntime::Start()
	{
		m_RuntimeScene = Engine::Scene::Copy(m_EditorScene);

		// find and set active camera
		auto cameras = m_RuntimeScene->GetEntities<Engine::CameraComponent>();
		if (!cameras.empty())
		{
			m_RuntimeScene->SetActiveCamera(cameras[0]);
		}

		Engine::SceneManager::SetActiveScene(m_RuntimeScene);

		for(const auto& ent : m_RuntimeScene->GetEntities<Engine::ScriptComponent>())
		{
			auto& sc = ent.Get<Engine::ScriptComponent>();
			if(sc.instance)
			{
				sc.instance->Self = ent;
				sc.instance->OnStart();
			}
		}

		m_Token = Engine::EventBus::Get().SubscribeToAll([](Engine::Event& e)
			{
				for (const auto& ent : m_RuntimeScene->GetEntities<Engine::ScriptComponent>())
				{
					auto& sc = ent.Get<Engine::ScriptComponent>();
					if (sc.instance)
					{
						sc.instance->OnEvent(e);
					}
				}
			});
	}

	void EditorRuntime::Stop()
	{
		for (const auto& ent : m_RuntimeScene->GetEntities<Engine::ScriptComponent>())
		{
			auto& sc = ent.Get<Engine::ScriptComponent>();
			if (sc.instance)
			{
				sc.instance->OnDestroy();
			}
		}

		Engine::EventBus::Get().Unsubscribe(m_Token);

		Engine::Audio::StopAll();
		Engine::SceneManager::SetActiveScene(m_EditorScene);
		delete m_RuntimeScene;
		m_RuntimeScene = nullptr;
		Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal);
	}

	void EditorRuntime::Update()
	{
		if (m_State != EditorState::Play) return;

		if(Engine::Input::IsKeyPressedOnce(Engine::KeyCode::Escape))
		{
			Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal);
		}

		// update scripts
		for (auto& ent : m_RuntimeScene->GetEntities<Engine::ScriptComponent>())
		{
			auto& scriptComp = ent.Get<Engine::ScriptComponent>();
			if (scriptComp.instance)
			{
				scriptComp.instance->OnUpdate();
			}
		}

		

		// update camera & audio listener
		for (auto& ent : m_RuntimeScene->GetEntities<Engine::CameraComponent>())
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