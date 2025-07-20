#include "enginepch.h"
#include "SceneManager.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Utils/File.h"
#include "Engine/Core/AssetManager.h"

namespace Engine::SceneManager
{
	Ref<Scene> s_ActiveSceneRef = nullptr;
	Scene* s_ActiveScene = new Scene("NewScene");

	void SceneManager::SetActiveScene(const Ref<Scene>& scene)
	{
		s_ActiveSceneRef = scene;
		s_ActiveScene->CopyFrom(*scene);
	}

	void CreateScene(const std::filesystem::path& path)
	{
		Ref<Scene> shared = AssetManager::Create<Scene>(path);
		SetActiveScene(shared);
	}

	void LoadScene()
	{
		std::string filePath = OpenFile("Scene (*.scene)\0*.scene\0");
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
		if (s_ActiveScene->GetFilePath().empty()) { SaveSceneAs(); return; }
		Serializer::Serialize(s_ActiveSceneRef);
		LOG_INFO("Scene saved to file {0}", s_ActiveScene->GetFilePath());
	}

	void SaveSceneAs()
	{
		std::string path = SaveFile("Scene (*.scene)\0*.scene\0");
		if (!path.empty())
		{
			s_ActiveScene->SetFilePath(path);
			Serializer::Serialize(s_ActiveSceneRef);
			LOG_INFO("Save as {0}", path);
		}
	}
}
