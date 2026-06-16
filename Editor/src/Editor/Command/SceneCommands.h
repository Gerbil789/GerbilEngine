#pragma once

#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Editor/Utility/File.h"
#include "Engine/Core/Project.h"
#include "Engine/Scene/Scene.h"

namespace Editor
{
	inline void SaveScene()
	{
		const Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
		auto& path = Engine::AssetManager::GetAssetRegistry().GetRecord(scene.id).path;

		if (path.empty())
		{
			path = Editor::FileDialog::SelectPath({ {"Scene Files", "*.scene"} }, "scene"); //prompt user to select path
		}

		Engine::SceneManager::SaveScene(Engine::Project::GetActive().GetAssetsDirectory() / path);
	}

  class OpenSceneCommand : public ICommand
  {
  public:
		OpenSceneCommand(Engine::Uuid id) : m_Id(id) {}

    void Execute() override
    {
			Engine::SceneManager::SetActiveScene(m_Id);
    }

    void Undo() override {}

  private:
    Engine::Uuid m_Id;
  };
}