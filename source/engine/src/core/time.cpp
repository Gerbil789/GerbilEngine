#include "enginepch.h"
#include "engine/core/time.h"
#include <chrono>

namespace engine
{
  namespace
  {
    static std::chrono::steady_clock::time_point s_lastTime = std::chrono::steady_clock::now();
  }

  void Time::BeginFrame()
  {
    auto now = std::chrono::steady_clock::now();
    s_deltaTime = std::chrono::duration<float>(now - s_lastTime).count();
    s_deltaTime = std::min(s_deltaTime, 0.1f);
    s_lastTime = now;
    s_FPS = (s_deltaTime > 0.0f) ? (1.0f / s_deltaTime) : 0.0f;
  }
}