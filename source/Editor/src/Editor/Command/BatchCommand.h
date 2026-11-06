#pragma once

#include "ICommand.h"
#include <vector>
#include <memory>

namespace editor
{
  class BatchCommand : public ICommand 
  {
  public:
    BatchCommand(std::vector<std::unique_ptr<ICommand>> commands) : m_commands(std::move(commands)) {}

    void Execute() override 
    {
      for (auto& cmd : m_commands) cmd->Execute();
    }

    void Undo() override 
    {
      for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it) {
        (*it)->Undo();
      }
    }

  private:
    std::vector<std::unique_ptr<ICommand>> m_commands;
  };
}