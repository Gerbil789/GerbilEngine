#pragma once

#include "Engine/Core/UUID.h"

namespace editor
{
  class EditorPicker
  {
  public:
    engine::Uuid Pick(uint32_t mouseX, uint32_t mouseY); //TODO: implement raycast based picking
  };
}