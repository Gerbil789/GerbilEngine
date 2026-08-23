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

			switch(Editor::editorContext.editorMode)
			{
				case EditorMode::Play:
				{
					Engine::Runtime::Start();
					SelectionManager::Entities.Clear();
					break;
				}
	
				case EditorMode::Edit:
				{
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