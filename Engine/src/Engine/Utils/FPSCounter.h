#pragma once

namespace Engine 
{
	class FPSCounter
  {
  public:
    explicit FPSCounter(size_t historySize = 30);
    void Update(float deltaTime);
    float GetAverageFPS() const;
  };
}