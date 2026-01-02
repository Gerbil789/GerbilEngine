#include "enginepch.h"
#include "SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utils/File.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

namespace Engine::SceneManager
{
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

	void SetActiveScene(Scene* scene)
	{
		if (!scene) 
		{
			LOG_ERROR("Setting active scene failed. Scene is null");
			return;
		}
		s_ActiveScene = scene;
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
		Scene* scene = AssetManager::CreateAsset<Scene>(path).get();
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

	void LoadScene(const std::filesystem::path&)
	{
		//Scene* scene = AssetManager::GetAsset<Scene>(path);
		//s_ActiveScene = scene;

	}

	void SaveScene()
	{
		if(s_ActiveScene == nullptr) 
		{ 
			LOG_WARNING("Saving scene failed. Current scene is nullptr"); 
			return; 
		}

		Scene* scene = AssetManager::GetAsset<Scene>(s_ActiveScene->id).get();
		auto assetPath = AssetManager::GetAssetPath(s_ActiveScene->id);

		if(assetPath.empty())
		{
			LOG_ERROR("SceneManager::SaveScene() - Current scene has no path");
			return;
		}

		auto path = Project::GetAssetsDirectory() / assetPath;

		SceneSerializer::Serialize(scene, path);

		LOG_INFO("Scene {0} saved to file {1}", s_ActiveScene->id, path);
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
