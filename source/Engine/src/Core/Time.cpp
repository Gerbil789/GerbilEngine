#include "enginepch.h"
#include "Engine/Core/Time.h"
#include <chrono>

namespace engine
{
  namespace
  {
    static float s_deltaTime = 0.0f;
    static float s_fPS = 0.0f;
    static std::chrono::steady_clock::time_point s_lastTime = std::chrono::steady_clock::now();
  }

  void Time::BeginFrame()
  {
    auto now = std::chrono::steady_clock::now();
    s_deltaTime = std::chrono::duration<float>(now - s_lastTime).count();
    s_deltaTime = std::min(s_deltaTime, 0.1f);
    s_lastTime = now;

    s_fPS = (s_deltaTime > 0.0f) ? (1.0f / s_deltaTime) : 0.0f;
  }

  float Time::DeltaTime()
  {
		return s_deltaTime;
  }

  float Time::FPS()
  {
		return s_fPS;
  }
}