#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Runtime.h"

namespace Editor
{
  class ChangeEditorStateCommand : public ICommand
  {
  public:
    ChangeEditorStateCommand(EditorMode newState) : m_State(newState) {}

    void Execute() override
    {
			Editor::editorContext.editorMode = m_State;

			Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());
			entt::registry& registry = scene.GetRegistry();

			switch(Editor::editorContext.editorMode)
			{
				case EditorMode::Play:
				{
					auto view = registry.view<Engine::CameraComponent, Engine::PrimaryCameraTag>(entt::exclude<Engine::EditorTag>);
					entt::entity gameCameraEntity = view.front();

					if (gameCameraEntity == entt::null) return;

					scene.SetActiveCamera(gameCameraEntity);

					//registry.emplace_or_replace<Engine::TransformDirty>(gameCameraEntity);
					//registry.emplace_or_replace<Engine::CameraProjectionDirty>(gameCameraEntity);
					//registry.emplace_or_replace<Engine::CameraViewDirty>(gameCameraEntity);

					Engine::Runtime::Start();
					SelectionManager::Entities.Clear();
					break;
				}
	
				case EditorMode::Edit:
				{
					auto view = registry.view<Engine::CameraComponent, Engine::EditorTag>();
					scene.SetActiveCamera(view.front());

					Engine::Runtime::Stop();
					break;
				}
			}
    }

    void Undo() override {}

  private:
		EditorMode m_State;
  };
}