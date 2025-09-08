#include "enginepch.h"
#include "SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utils/File.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine::SceneManager
{
	//The scene is owned by asset manager, we don't use Ref<> here to avoid circular reference
	Scene* s_ActiveScene = nullptr;
	static std::vector<SceneChangedCallback> s_Callbacks;

	void RegisterOnSceneChanged(const SceneChangedCallback& callback)
	{
		s_Callbacks.push_back(callback);
	}

	void NotifySceneChanged()
	{
		for (auto& callback : s_Callbacks)
		{
			callback(s_ActiveScene);
		}
	}

	void SetActiveScene(Ref<Scene> scene)
	{
		if (!scene) 
		{
			LOG_ERROR("Setting active scene failed. Scene is null");
			return;
		}
		s_ActiveScene = scene.get();
		LOG_INFO("Active scene set to {0}", s_ActiveScene->id);
		NotifySceneChanged();
	}

	Scene* GetActiveScene()
	{
		if (!s_ActiveScene) {
			LOG_ERROR("Getting active scene failed. Current scene is null");
			return nullptr;
		}
		return s_ActiveScene;
	}

	void CreateScene(const std::filesystem::path& path)
	{
		Ref<Scene> scene = AssetManager::CreateAsset<Scene>(path);
		SetActiveScene(scene);
	}

	void LoadScene()
	{
		std::string filePath = OpenFile();
		if (filePath.empty()) 
		{ 
			LOG_ERROR("Failed to open file {0}", filePath); return; 
		}
		LoadScene(filePath);
	}

	void LoadScene(const std::filesystem::path& path)
	{
		//Ref<Scene> scene = AssetManager::GetAsset<Scene>(path);
		//s_ActiveScene = scene;
		//AssetManager::UnloadUnusedAssets();

	}

	void SaveScene()
	{
		if(s_ActiveScene == nullptr) 
		{ 
			LOG_WARNING("Saving scene failed. Current scene is nullptr"); 
			return; 
		}

		//if (s_ActiveScene->GetPath().empty()) { SaveSceneAs(); return; }
		//Serializer::Serialize(s_ActiveScene);
		//LOG_INFO("Scene saved to file {0}", s_ActiveScene->GetPath());
		LOG_WARNING("Saving scene failed. NOT IMPLEMENTED!");
	}

	void SaveSceneAs()
	{
		std::string path = SaveFile();
		if (!path.empty())
		{
			//s_ActiveScene->SetFilePath(path);
			//Serializer::Serialize(s_ActiveScene);
			//LOG_INFO("Save as {0}", path);

			LOG_WARNING("Saving scene failed. NOT IMPLEMENTED!");
		}
	}
}
