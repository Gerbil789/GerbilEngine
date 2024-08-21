#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Utils/PlatformUtils.h"

namespace Engine
{
	Ref<Scene> SceneManager::s_CurrentScene = nullptr;
	std::vector<ISceneObserver*> SceneManager::s_Observers;

	void SceneManager::CreateScene(const std::string& filePath)
	{
		s_CurrentScene = CreateRef<Scene>(filePath);
		NotifyObservers();
	}

	void SceneManager::LoadScene()
	{
		std::string filePath = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
		if (filePath.empty()) { ENGINE_LOG_ERROR("Failed to open file {0}", filePath); return; }
		LoadScene(filePath);
	}

	void SceneManager::LoadScene(const std::string& filePath)
	{
		Ref<Scene> scene = CreateRef<Scene>(filePath);
		Serializer::Deserialize(scene);
		if(scene == nullptr) { ENGINE_LOG_ERROR("Failed to load scene from file {0}", filePath); }
		s_CurrentScene = scene;
		NotifyObservers();
	}

	void SceneManager::SaveScene()
	{
		if(s_CurrentScene == nullptr) { ENGINE_LOG_ERROR("Saving scene failed. Current scene is null"); return; }
		if (s_CurrentScene->GetFilePath().empty()) { SaveSceneAs(); return; }
		Serializer::Serialize(s_CurrentScene);
		ENGINE_LOG_INFO("Scene saved to file {0}", s_CurrentScene->GetFilePath());
	}

	void SceneManager::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!path.empty())
		{
			s_CurrentScene->SetFilePath(path);
			Serializer::Serialize(s_CurrentScene);
			ENGINE_LOG_INFO("Save as {0}", path);
		}
	}

	void SceneManager::NotifyObservers()
	{
		for (auto observer : s_Observers) 
		{
			observer->OnSceneChanged();
		}
	}
}
