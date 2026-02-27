#include "EditorRuntime.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Script/Script.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/Camera.h"
#include <Windows.h>

namespace Editor
{
	namespace 
	{
		bool m_IgnoredSceneChange = false;
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
		Engine::SceneManager::SetActiveScene(m_RuntimeScene);

		for(const auto& ent : m_RuntimeScene->GetEntities<Engine::ScriptComponent>())
		{
			auto& sc = ent.GetComponent<Engine::ScriptComponent>();
			if(sc.instance)
			{
				sc.instance->Self = ent;
				sc.instance->OnStart();
			}
		}
	}

	void EditorRuntime::Stop()
	{
		Engine::Audio::StopAll();
		Engine::SceneManager::SetActiveScene(m_EditorScene);
		delete m_RuntimeScene;
		m_RuntimeScene = nullptr;
	}

	void EditorRuntime::Update()
	{
		// update scripts
		for (auto& ent : m_RuntimeScene->GetEntities<Engine::ScriptComponent>())
		{
			auto& scriptComp = ent.GetComponent<Engine::ScriptComponent>();
			if (scriptComp.instance)
			{
				scriptComp.instance->OnUpdate();
			}
		}

		// update audio listener
		for (auto& ent : m_RuntimeScene->GetEntities<Engine::CameraComponent>())
		{
			Engine::Camera* cam = ent.GetComponent<Engine::CameraComponent>().camera;
			const auto& pos = ent.GetComponent<Engine::TransformComponent>().position;
			const auto& forward = cam->GetForward();
			const auto& up = cam->GetUp();
			Engine::Audio::SetListener(pos.x, pos.y, pos.z, forward.x, forward.y, forward.z, up.x, up.y, up.z);
		}
	}
}