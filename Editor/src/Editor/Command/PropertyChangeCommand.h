#pragma once

#include "ICommand.h"
#include <functional>

namespace Editor
{
  class PropertyChangeCommand : public ICommand
  {
  public:
    using ApplyFn = std::function<void()>;

    PropertyChangeCommand(ApplyFn apply, ApplyFn undo) : m_Apply(apply), m_Undo(undo) {}

    void Execute() override { m_Apply(); }
    void Undo() override { m_Undo(); }

  private:
    ApplyFn m_Apply;
    ApplyFn m_Undo;
  };
}

