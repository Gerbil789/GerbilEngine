#pragma once

namespace engine
{
  struct Time
  { 
    static void BeginFrame();
    inline static float DeltaTime() { return s_deltaTime; }
    inline static float FPS() { return s_FPS; }

  private:
    inline static float s_deltaTime = 0.0f;
    inline static float s_FPS = 0.0f;
  };
}