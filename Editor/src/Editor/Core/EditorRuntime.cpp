#include "EditorRuntime.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Script/Script.h"
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



		auto entities = m_RuntimeScene->GetEntities<Engine::ScriptComponent>();
		for(auto ent : entities)
		{
			auto& sc = ent.GetComponent<Engine::ScriptComponent>();
			sc.instance->Self = ent;
		}

		/*if (m_GameInstance)
		{
			LOG_WARNING("Runtime already started");
			return;
		}

		m_GameInstance = std::make_unique<GameInstance>();

		m_GameInstance->OnExit = []() { m_GameInstance.reset(); };
		Engine::Scene* activeScene = Engine::SceneManager::GetActiveScene();
		m_GameInstance->Initialize(activeScene);*/
	}

	void EditorRuntime::Stop()
	{
		Engine::SceneManager::SetActiveScene(m_EditorScene);
		delete m_RuntimeScene;
		m_RuntimeScene = nullptr;

		/*if (!m_GameInstance)
		{
			LOG_WARNING("Runtime not running");
			return;
		}

		m_GameInstance->Close();
		m_GameInstance.reset();*/
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


		//Engine::Camera* camera = m_ActiveCameraEntity.GetComponent<Engine::CameraComponent>().camera;
		//camera->SetPosition(m_ActiveCameraEntity.GetComponent<Engine::TransformComponent>().position);
		//camera->SetRotation(m_ActiveCameraEntity.GetComponent<Engine::TransformComponent>().rotation);


		/*if (m_GameInstance)
		{
			m_GameInstance->Update();
		}*/
	}
}