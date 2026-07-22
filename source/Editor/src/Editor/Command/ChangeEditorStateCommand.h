#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/SelectionManager.h"
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

			switch(m_State)
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