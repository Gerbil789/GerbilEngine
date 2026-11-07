#pragma once

namespace engine
{
  struct Time
  {
    static void BeginFrame();
    static float DeltaTime();
    static float FPS();
  };
}