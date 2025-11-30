#pragma once

#include <chrono>

namespace Engine
{
  class Time
  {
  public:
    static void BeginFrame();
		static float DeltaTime() { return s_DeltaTime; }
		static float FPS() { return s_FPS; }

  private:
    static inline float s_DeltaTime = 0.0f;
    static inline float s_FPS = 0.0f;
    static inline std::chrono::steady_clock::time_point s_LastTime;
  };
}