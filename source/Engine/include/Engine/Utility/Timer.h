#pragma once

#include "Engine/Core/Log.h"
#include <chrono>
#include <string>

namespace engine
{
  class ScopedTimer
  {
  public:
    explicit ScopedTimer(std::string name = "elapsed") : m_name(std::move(name)), m_startTime(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer()
    {
      auto endTime = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed = endTime - m_startTime;

      LOG_INFO("{}: {:.3f} ms", m_name, elapsed.count());
    }

  private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_startTime;
  };
}