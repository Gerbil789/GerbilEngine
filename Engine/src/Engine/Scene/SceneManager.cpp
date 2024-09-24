#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Core/AssetManager.h"

namespace Engine
{
	Ref<Scene> SceneManager::s_CurrentScene = nullptr;
	std::vector<ISceneObserver*> SceneManager::s_Observers;

	void SceneManager::CreateScene(const std::filesystem::path& path)
	{
		s_CurrentScene = AssetManager::CreateAsset<Scene>(path);
		NotifyObservers();
	}

	void SceneManager::LoadScene()
	{
		std::string filePath = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
		if (filePath.empty()) { LOG_ERROR("Failed to open file {0}", filePath); return; }
		LoadScene(filePath);
	}

	void SceneManager::LoadScene(const std::filesystem::path& path)
	{
		Ref<Scene> scene = AssetManager::GetAsset<Scene>(path);
		s_CurrentScene->OnDestroy();
		s_CurrentScene = scene;
		AssetManager::UnloadUnusedAssets();
		NotifyObservers();
	}

	void SceneManager::SaveScene()
	{
		if(s_CurrentScene == nullptr) { LOG_ERROR("Saving scene failed. Current scene is null"); return; }
		if (s_CurrentScene->GetFilePath().empty()) { SaveSceneAs(); return; }
		Serializer::Serialize(s_CurrentScene);
		LOG_INFO("Scene saved to file {0}", s_CurrentScene->GetFilePath());
	}

	void SceneManager::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!path.empty())
		{
			s_CurrentScene->SetFilePath(path);
			Serializer::Serialize(s_CurrentScene);
			LOG_INFO("Save as {0}", path);
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
