#include "enginepch.h"
#include "Engine/Utility/FPSCounter.h"

namespace engine 
{
  size_t m_frameHistorySize;
  std::vector<float> m_fPSHistory;
  size_t m_fPSHistoryIndex;
  float m_fPSHistorySum;
  float m_averageFPS;

  FPSCounter::FPSCounter(size_t historySize)
  {
		m_frameHistorySize = historySize;
		m_fPSHistory.resize(historySize, 0.0f);
		m_fPSHistoryIndex = 0;
		m_fPSHistorySum = 0.0f;
		m_averageFPS = 0.0f;
  }

  void FPSCounter::Update(float deltaTime)
  {
    float currentFPS = (deltaTime > 0.0f) ? 1.0f / deltaTime : 0.0f;

    m_fPSHistorySum -= m_fPSHistory[m_fPSHistoryIndex];
    m_fPSHistory[m_fPSHistoryIndex] = currentFPS;
    m_fPSHistorySum += currentFPS;

    m_fPSHistoryIndex = (m_fPSHistoryIndex + 1) % m_frameHistorySize;

    m_averageFPS = m_fPSHistorySum / static_cast<float>(m_frameHistorySize);
  }

  float FPSCounter::GetAverageFPS() const
  {
    return m_averageFPS;
  }
}