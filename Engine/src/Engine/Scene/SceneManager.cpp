#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Utils/PlatformUtils.h"

namespace Engine
{
	std::string SceneManager::s_CurrentScenePath = "";
	Ref<Scene> SceneManager::s_CurrentScene = nullptr;
	std::vector<ISceneObserver*> SceneManager::s_Observers;

	void SceneManager::CreateScene(const std::string& name)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		s_CurrentScene = newScene;
		s_CurrentScenePath = std::string();

		NotifyObservers();
	}

	void SceneManager::LoadScene()
	{
		std::string path = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
		if (path.empty()) { ENGINE_LOG_ERROR("Failed to open file {0}", path); return; }
		LoadScene(path);
	}

	void SceneManager::LoadScene(const std::string& filepath)
	{
		Ref<Scene> newScene = DeserializeScene(filepath);
		if(newScene == nullptr) { ENGINE_LOG_ERROR("Failed to load scene from file {0}", filepath); }

		s_CurrentScene = newScene;
		s_CurrentScenePath = filepath;

		NotifyObservers();
	}

	void SceneManager::SaveScene()
	{
		if (s_CurrentScenePath.empty()) { ENGINE_LOG_ERROR("File path is empty"); return; }
		SerializeScene(s_CurrentScene, s_CurrentScenePath);
		ENGINE_LOG_INFO("Scene saved to file {0}", s_CurrentScenePath);
	}

	void SceneManager::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!path.empty())
		{
			SerializeScene(s_CurrentScene, path);
			ENGINE_LOG_INFO("Save as {0}", path);
		}
	}

	void SceneManager::SerializeScene(const Ref<Scene>& scene, const std::string& filepath)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(filepath);
	}

	Ref<Scene> SceneManager::DeserializeScene(const std::string& filepath)
	{
		if (filepath.empty()) { ENGINE_LOG_ERROR("File path is empty"); return nullptr; }

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);

		if (!serializer.Deserialize(filepath)) { ENGINE_LOG_ERROR("Failed to deserialize file {0}", filepath);  return nullptr; }

		return newScene;
	}

	void SceneManager::NotifyObservers()
	{
		for (auto observer : s_Observers) 
		{
			observer->OnSceneChanged();
		}
	}
}
