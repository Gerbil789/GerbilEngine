#pragma once

namespace Engine
{
  class Time
  {
  public:
    static void BeginFrame();
    static float DeltaTime();
    static float FPS();
  };
}