#include "enginepch.h"
#include "Time.h"
#include <chrono>

namespace Engine
{
  static float s_DeltaTime = 0.0f;
  static float s_FPS = 0.0f;
  static std::chrono::steady_clock::time_point s_LastTime;

  void Time::BeginFrame()
  {
    auto now = std::chrono::steady_clock::now();
    s_DeltaTime = std::chrono::duration<float>(now - s_LastTime).count();
    s_LastTime = now;

    s_FPS = 1.0f / s_DeltaTime;
  }

  float Time::DeltaTime()
  {
		return s_DeltaTime;
  }

  float Time::FPS()
  {
		return s_FPS;
  }
}