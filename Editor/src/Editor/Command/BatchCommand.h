#pragma once

#include "ICommand.h"
#include <vector>
#include <memory>

namespace Editor
{
  class BatchCommand : public ICommand 
  {
  public:
    BatchCommand(std::vector<std::unique_ptr<ICommand>> commands) : m_Commands(std::move(commands)) {}

    void Execute() override 
    {
      for (auto& cmd : m_Commands) cmd->Execute();
    }

    void Undo() override 
    {
      for (auto it = m_Commands.rbegin(); it != m_Commands.rend(); ++it) {
        (*it)->Undo();
      }
    }

  private:
    std::vector<std::unique_ptr<ICommand>> m_Commands;
  };
}