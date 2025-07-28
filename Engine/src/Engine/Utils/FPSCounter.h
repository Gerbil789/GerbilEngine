#pragma once

#include <vector>

namespace Engine 
{
	class FPSCounter //TODO: make fps counter better overall, and move it into debug directory
  {
  public:
    explicit FPSCounter(size_t historySize = 30);

    void Update(float deltaTime);

    float GetAverageFPS() const;

  private:
    size_t m_FrameHistorySize;
    std::vector<float> m_FPSHistory;
    size_t m_FPSHistoryIndex;
    float m_FPSHistorySum;
    float m_AverageFPS;
  };
}