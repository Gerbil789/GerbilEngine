#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/UUID.h"

namespace Engine::SceneManager
{
	Scene* s_ActiveScene = nullptr;
	static std::vector<std::function<void(Scene*)>> s_Callbacks;

	void RegisterOnSceneChanged(const std::function<void(Scene*)>& callback)
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
		LOG_INFO("Active scene set to {}", s_ActiveScene->id);
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

	void LoadScene(Uuid id)
	{
		if (!id)
		{
			throw std::runtime_error("No valid start scene set in the project");
		}

		Engine::Scene* scene = Engine::AssetManager::GetAsset<Engine::Scene>(id);
		Engine::SceneManager::SetActiveScene(scene);

		auto entities = scene->GetEntities(true);
		LOG_INFO("Loaded scene with {} entities", entities.size());

		for (Entity ent : entities)
		{
			LOG_INFO("  {} \t{}", ent.GetName(), ent.GetUUID());
		}
	}

	void SaveScene()
	{
		if(s_ActiveScene == nullptr) 
		{ 
			LOG_WARNING("Saving scene failed. Current scene is nullptr"); 
			return; 
		}

		Scene* scene = AssetManager::GetAsset<Scene>(s_ActiveScene->id);
		auto assetPath = AssetManager::GetAssetPath(s_ActiveScene->id);

		if(assetPath.empty())
		{
			LOG_ERROR("SceneManager::SaveScene() - Current scene has no path");
			return;
		}

		auto path = Engine::GetAssetsDirectory() / assetPath;

		SceneSerializer::Serialize(scene, path);

		LOG_INFO("Scene {} saved to file {}", s_ActiveScene->id, path);
	}

	void SaveSceneAs()
	{
		LOG_WARNING("Saving scene failed. NOT IMPLEMENTED!");

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
