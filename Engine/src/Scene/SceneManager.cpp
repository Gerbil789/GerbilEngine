#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"

namespace Engine::SceneManager
{
	static Scene s_ActiveScene;

	void SetActiveScene(Scene& scene)
	{
		s_ActiveScene = std::move(scene);
		LOG_INFO("Active scene set to {}", s_ActiveScene.id);
	}

	Scene& GetActiveScene()
	{
		return s_ActiveScene;
	}

	void SaveScene()
	{
		auto assetPath = Engine::AssetManager::GetAssetRegistry().GetPath(s_ActiveScene.id);

		if(assetPath.empty())
		{
			LOG_ERROR("Current scene has no path");
			return;
		}

		auto path = Engine::Project::GetActive().GetAssetsDirectory() / assetPath;

		SceneSerializer::Serialize(s_ActiveScene, path);

		LOG_INFO("Scene {} saved to file {}", s_ActiveScene.id, path);
	}

	void SaveSceneAs()
	{
		LOG_WARNING("NOT IMPLEMENTED!");

		//std::string path = SaveFile();
		//if (!path.empty())
		//{
		//	//s_ActiveScene->SetFilePath(path);
		//	//Serializer::Serialize(s_ActiveScene);
		//	//LOG_INFO("Save as {0}", path);

		//	LOG_WARNING("Saving scene failed. NOT IMPLEMENTED!");
		//}
	}
}
