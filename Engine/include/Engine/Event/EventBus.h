#pragma once

#include "Engine/Core/API.h"
#include "Engine/Event/Event.h"
#include <unordered_map>
#include <functional>

namespace Engine
{
  using EventToken = uint32_t;

  class ENGINE_API EventBus
  {
  public:
    static EventBus& Get()
    {
      static EventBus instance;
      return instance;
    }

    template<typename EventType>
    EventToken Subscribe(std::function<void(const EventType&)> callback)
    {
      auto wrapper = [callback](Engine::Event& e)
        {
          if (e.GetEventType() == EventType::GetStaticType())
          {
            callback(static_cast<const EventType&>(e));
          }
        };

      EventToken id = ++m_NextToken;
      m_Subscribers[id] = wrapper;
      return id;
    }

    EventToken SubscribeToAll(std::function<void(Engine::Event&)> callback)
    {
      auto wrapper = [callback](Engine::Event& e)
        {
          callback(e);
        };

      EventToken id = ++m_NextToken;
      m_Subscribers[id] = wrapper;
      return id;
    }

    void Unsubscribe(EventToken token)
    {
      m_Subscribers.erase(token);
    }

    void Publish(Engine::Event& e)
    {
      for (auto& [id, sub] : m_Subscribers)
      {
        sub(e);
        // Optional: If you want to stop propagating the event once it's handled
        // if (e.Handled) break; 
      }
    }

  private:
    EventBus() = default;
    ~EventBus() = default;

    EventToken m_NextToken = 0;
    std::unordered_map<EventToken, std::function<void(Engine::Event&)>> m_Subscribers;
  };
}