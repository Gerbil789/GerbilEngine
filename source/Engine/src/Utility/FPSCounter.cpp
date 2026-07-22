#include "enginepch.h"
#include "Engine/Utility/FPSCounter.h"

namespace Engine 
{
  size_t m_FrameHistorySize;
  std::vector<float> m_FPSHistory;
  size_t m_FPSHistoryIndex;
  float m_FPSHistorySum;
  float m_AverageFPS;

  FPSCounter::FPSCounter(size_t historySize)
  {
		m_FrameHistorySize = historySize;
		m_FPSHistory.resize(historySize, 0.0f);
		m_FPSHistoryIndex = 0;
		m_FPSHistorySum = 0.0f;
		m_AverageFPS = 0.0f;
  }

  void FPSCounter::Update(float deltaTime)
  {
    float currentFPS = (deltaTime > 0.0f) ? 1.0f / deltaTime : 0.0f;

    m_FPSHistorySum -= m_FPSHistory[m_FPSHistoryIndex];
    m_FPSHistory[m_FPSHistoryIndex] = currentFPS;
    m_FPSHistorySum += currentFPS;

    m_FPSHistoryIndex = (m_FPSHistoryIndex + 1) % m_FrameHistorySize;

    m_AverageFPS = m_FPSHistorySum / static_cast<float>(m_FrameHistorySize);
  }

  float FPSCounter::GetAverageFPS() const
  {
    return m_AverageFPS;
  }
}