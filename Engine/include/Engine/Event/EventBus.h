#pragma once

#include "Engine/Core/API.h"
#include "Engine/Event/Event.h"

namespace Engine
{
  class ENGINE_API EventBus
  {
  public:
    static EventBus& Get()
    {
      static EventBus instance;
      return instance;
    }

    template<typename EventType>
    void Subscribe(std::function<void(const EventType&)> callback)
    {
      auto wrapper = [callback](Engine::Event& e)
        {
          if (e.GetEventType() == EventType::GetStaticType())
          {
            callback(static_cast<const EventType&>(e));
          }
        };
      m_Subscribers.push_back(wrapper);
    }

    void Publish(Engine::Event& e)
    {
      for (auto& sub : m_Subscribers)
      {
        sub(e);
      }
    }

  private:
    EventBus() = default;
    ~EventBus() = default;

    std::vector<std::function<void(Engine::Event&)>> m_Subscribers;
  };
}