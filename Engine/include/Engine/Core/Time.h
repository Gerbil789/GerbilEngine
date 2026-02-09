#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
  class ENGINE_API Time
  {
  public:
    static void BeginFrame();
    static float DeltaTime();
    static float FPS();
  };
}