#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine 
{
	class ISceneObserver
	{
	public:
		virtual ~ISceneObserver() = default;
		virtual void OnSceneChanged() = 0;
	};

	class SceneManager
	{
	public:
		SceneManager() = default;
		~SceneManager() = default;

		static Ref<Scene> GetCurrentScene() { return s_CurrentScene; }
		static void CreateScene(const std::string& name = "Untitled Scene");
		static void LoadScene();
		static void LoadScene(const std::string& filepath);
		static void SaveScene();
		static void SaveSceneAs();

		static void AddObserver(ISceneObserver* observer) { s_Observers.push_back(observer); }
		static void RemoveObserver(ISceneObserver* observer) { s_Observers.erase(std::remove(s_Observers.begin(), s_Observers.end(), observer), s_Observers.end()); }

	private:
		static void SerializeScene(const Ref<Scene>& scene, const std::string& filepath);
		static Ref<Scene> DeserializeScene(const std::string& filepath);

		static void NotifyObservers();

	private:
		static std::string s_CurrentScenePath;
		static Ref<Scene> s_CurrentScene;
		static std::vector<ISceneObserver*> s_Observers;
	};
}