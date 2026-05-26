#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"
#include "Engine/Utility/Path.h"

namespace Engine::SceneManager
{
	static Scene* s_ActiveScene = nullptr;

	void SetActiveScene(Uuid id)
	{
		if(s_ActiveScene && s_ActiveScene->id == id)
		{
			LOG_WARNING("Scene {} is already active", id);
			return;
		}

		auto& scene = AssetManager::GetAsset<Scene>(id);
		s_ActiveScene = &scene;
		LOG_INFO("Active scene set to {}", id);
	}

	Scene& GetActiveScene()
	{
		return *s_ActiveScene;
	}

	void SaveScene(const std::filesystem::path& path)
	{
		SceneSerializer::Serialize(*s_ActiveScene, path);
		LOG_INFO("Scene {} saved to file {}", s_ActiveScene->id, path);
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
