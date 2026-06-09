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
    ChangeEditorStateCommand(EditorState newState) : m_State(newState) {}

    void Execute() override
    {
			EditorContext::state = m_State;

			switch(m_State)
			{
				case EditorState::Play:
				{
					Engine::Runtime::Start();
					SelectionManager::Entities.Clear();
					break;
				}
	
				case EditorState::Edit:
				{
					Engine::Runtime::Stop();
					break;
				}
			}
    }

    void Undo() override {}

  private:
		EditorState m_State;
  };
}