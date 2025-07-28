#include "enginepch.h"
#include "SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Utils/File.h"
#include "Engine/Core/AssetManager.h"

namespace Engine::SceneManager
{
	Scene* s_ActiveScene = nullptr;

	void SetActiveScene(Scene* scene)
	{
		if (scene == nullptr) { LOG_ERROR("Setting active scene failed. Scene is null"); return; }
		s_ActiveScene = scene;
		LOG_INFO("Active scene set to {0}", s_ActiveScene->GetPath());
	}

	Scene* GetActiveScene()
	{
		if (s_ActiveScene == nullptr) { LOG_ERROR("Getting active scene failed. Current scene is null"); return nullptr; }
		return s_ActiveScene;
	}

	void CreateScene(const std::filesystem::path& path)
	{
		Ref<Scene> shared = AssetManager::Create<Scene>(path);
		SetActiveScene(shared.get());
	}

	void LoadScene()
	{
		std::string filePath = OpenFile();
		if (filePath.empty()) { LOG_ERROR("Failed to open file {0}", filePath); return; }
		LoadScene(filePath);
	}

	void LoadScene(const std::filesystem::path& path)
	{
		Ref<Scene> scene = AssetManager::Get<Scene>(path);
		//s_ActiveScene = scene;
		//AssetManager::UnloadUnusedAssets();

	}

	void SaveScene()
	{
		if(s_ActiveScene == nullptr) { LOG_ERROR("Saving scene failed. Current scene is null"); return; }
		if (s_ActiveScene->GetPath().empty()) { SaveSceneAs(); return; }
		Serializer::Serialize(s_ActiveScene);
		LOG_INFO("Scene saved to file {0}", s_ActiveScene->GetPath());
	}

	void SaveSceneAs()
	{
		std::string path = SaveFile();
		if (!path.empty())
		{
			//s_ActiveScene->SetFilePath(path);
			Serializer::Serialize(s_ActiveScene);
			LOG_INFO("Save as {0}", path);
		}
	}
}
