#pragma once

namespace engine 
{
	class FPSCounter
  {
  public:
    explicit FPSCounter(size_t historySize = 30);
    void Update(float deltaTime);
    float GetAverageFPS() const;
  };
}