#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Core/Runtime.h"

namespace editor
{
  class ChangeEditorStateCommand : public ICommand
  {
  public:
    ChangeEditorStateCommand(EditorMode newState) : m_state(newState) {}

    void Execute() override
    {
			editor::context::editorMode = m_state;

			switch(editor::context::editorMode)
			{
				case EditorMode::Play:
				{
					engine::Runtime::Start();
					// SelectionManager::Entities.Clear();
					break;
				}
	
				case EditorMode::Edit:
				{
					engine::Runtime::Stop();
					break;
				}
			}
    }

    void Undo() override {}

  private:
		EditorMode m_state;
  };
}