#include "enginepch.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include <Windows.h>

namespace Engine
{
	void Runtime::LoadScripts(ScriptRegistry& registry, const std::filesystem::path& dllPath)
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

	void Runtime::Start()
	{
		if (m_GameInstance)
		{
			LOG_WARNING("Runtime already started");
			return;
		}

		m_GameInstance = std::make_unique<GameInstance>();

		m_GameInstance->OnExit = []() { m_GameInstance.reset(); };
		Engine::Scene* activeScene = Engine::SceneManager::GetActiveScene();
		m_GameInstance->Initialize(activeScene);
	}

	void Runtime::Stop()
	{
		if (!m_GameInstance)
		{
			LOG_WARNING("Runtime not running");
			return;
		}

		m_GameInstance->Close();
		m_GameInstance.reset();
	}

	void Runtime::Update()
	{
		if (m_GameInstance)
		{
			m_GameInstance->Update();
		}
	}
}