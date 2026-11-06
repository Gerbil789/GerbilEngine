#pragma once

#include "ICommand.h"
#include <functional>

namespace editor
{
  class PropertyChangeCommand : public ICommand
  {
  public:
    using ApplyFn = std::function<void()>;

    PropertyChangeCommand(ApplyFn apply, ApplyFn undo) : m_apply(apply), m_undo(undo) {}

    void Execute() override { m_apply(); }
    void Undo() override { m_undo(); }

  private:
    ApplyFn m_apply;
    ApplyFn m_undo;
  };
}

