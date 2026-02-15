#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/GameInstance.h"

namespace Engine
{
	class ScriptRegistry;

	class ENGINE_API Runtime
	{
	public:
		static void LoadScripts(ScriptRegistry& registry, const std::filesystem::path& dllPath);

		static void Start();
		static void Stop();
		static void Update();

	private:
		inline static std::unique_ptr<GameInstance> m_GameInstance = nullptr;
	};
}