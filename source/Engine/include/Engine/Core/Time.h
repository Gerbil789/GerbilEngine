#pragma once

namespace engine
{
  class Time
  {
  public:
    static void BeginFrame();
    static float DeltaTime();
    static float FPS();
  };
}