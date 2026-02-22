#pragma once

//#include "Engine/Core/GameInstance.h"
#include <filesystem>
#include <memory>

namespace Engine 
{
	class Scene;
	class ScriptRegistry;
}

namespace Editor
{
	enum class EditorState { Edit, Play };
	enum class PlayMode { PIE, Windowed, Fullscreen }; // PIE - Play In Editor

	class EditorRuntime
	{
	public:
		static void Initialize();
		static void LoadScripts(Engine::ScriptRegistry& registry, const std::filesystem::path& dllPath);
		static void Update();
		static void SetEditorState(EditorState newState);
		static EditorState GetState() { return m_State; }

	private:
		static void Start();
		static void Stop();

	private:
		inline static EditorState m_State = EditorState::Edit;
		inline static PlayMode m_PlayMode = PlayMode::PIE;

		inline static Engine::Scene* m_EditorScene = nullptr;
		inline static Engine::Scene* m_RuntimeScene = nullptr;

		//inline static std::unique_ptr<GameInstance> m_GameInstance = nullptr;





	};
}

