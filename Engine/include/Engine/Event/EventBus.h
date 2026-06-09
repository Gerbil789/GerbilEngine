#pragma once

#include "Engine/Core/API.h"
#include "Engine/Event/Event.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace Engine
{
  using EventToken = uint32_t;

  template<typename T>
  concept IsEvent = std::is_base_of_v<Event, std::remove_cvref_t<T>>;

  class ENGINE_API EventBus
  {
  private:
    struct Handler
    {
      EventToken Token;
      std::function<void(Engine::Event&)> Callback;
    };

  public:
    static EventBus& Get()
    {
      static EventBus instance;
      return instance;
    }

    template<IsEvent T>
    EventToken Subscribe(std::function<void(const T&)> callback)
    {
      size_t typeHash = typeid(T).hash_code();

      auto wrapper = [callback](Engine::Event& e)
        {
          callback(static_cast<const T&>(e));
        };

      EventToken id = ++m_NextToken;
      m_Subscribers[typeHash].push_back({ id, wrapper });
      return id;
    }

    void Unsubscribe(EventToken token)
    {
      for (auto& [hash, handlers] : m_Subscribers)
      {
        std::erase_if(handlers, [token](const Handler& h) { return h.Token == token; });
      }
    }

    template<IsEvent T>
    void Publish(T&& e)
    {
      size_t typeHash = typeid(e).hash_code();

      if (!m_Subscribers.contains(typeHash)) return;

      for (auto& handler : m_Subscribers[typeHash])
      {
        if (e.Handled) break; // Support for consuming events
        handler.Callback(e);
      }
    }

  private:
    EventBus() = default;
    ~EventBus() = default;

    EventToken m_NextToken = 0;
    std::unordered_map<size_t, std::vector<Handler>> m_Subscribers;
  };
}