#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine 
{
	class ISceneObserver
	{
	public:
		virtual ~ISceneObserver() = default;
		virtual void OnSceneChanged(Ref<Scene> newScene) = 0;
	};

	class SceneManager
	{
	public:
		SceneManager() = default;
		~SceneManager() = default;

		static Ref<Scene> GetCurrentScene() { return s_CurrentScene; }
		static void CreateScene(const std::filesystem::path& path);
		static void LoadScene();
		static void LoadScene(const std::filesystem::path& path);
		static void SaveScene();
		static void SaveSceneAs();

		static void RegisterObserver(ISceneObserver* observer) { s_Observers.insert(observer); observer->OnSceneChanged(s_CurrentScene); }
		static void UnregisterObserver(ISceneObserver* observer) { s_Observers.erase(observer); }

	private:
		static void NotifyObservers();

	private:
		static inline Ref<Scene> s_CurrentScene = nullptr;
		static inline std::unordered_set<ISceneObserver*> s_Observers;
	};
}