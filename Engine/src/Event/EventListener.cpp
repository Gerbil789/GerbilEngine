#include "enginepch.h"
#include "Engine/Event/EventListener.h"
#include "Engine/Event/EventBus.h"

namespace Engine
{
  void EventListener::Release()
  {
    if (m_Token)
    {
      EventBus::Unsubscribe(m_Token);
      m_Token = 0;
    }
  }
}