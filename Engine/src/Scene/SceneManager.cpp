#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"
#include "Engine/Utility/Path.h"

namespace Engine::SceneManager
{
	static Uuid m_ActiveScene;

	void SetActiveScene(Uuid id)
	{
		if(m_ActiveScene == id)
		{
			LOG_WARNING("Scene {} is already active", id);
			return;
		}

		m_ActiveScene = id;
		LOG_INFO("Active scene set to {}", m_ActiveScene);
	}

	Scene& GetActiveScene()
	{
		return AssetManager::GetAsset<Scene>(m_ActiveScene);
	}

	void SaveScene(const std::filesystem::path& path)
	{
		SceneSerializer::Serialize(AssetManager::GetAsset<Scene>(m_ActiveScene), path);
		LOG_INFO("Scene {} saved to file {}", m_ActiveScene, path);
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
