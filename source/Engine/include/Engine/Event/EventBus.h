#pragma once

#include "Engine/Event/Event.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace engine
{
  template<typename T>
  concept IsEvent = std::is_base_of_v<Event, std::remove_cvref_t<T>>;

  class EventBus
  {
  public:
    EventBus() = delete;

  private:
    struct Handler
    {
      uint32_t Token = 0;
      std::function<bool(Event&)> Callback;
    };

    static inline uint32_t s_NextToken = 0;
    static inline std::unordered_map<size_t, std::vector<Handler>> s_Subscribers;

  public:
    template<IsEvent T>
    static uint32_t Subscribe(std::function<bool(const T&)> callback)
    {
      size_t typeHash = typeid(T).hash_code();

      uint32_t token = ++s_NextToken;

      Handler h;
      h.Token = token;

      h.Callback = [callback](Event& e) -> bool
        {
          return callback(static_cast<const T&>(e));
        };

      s_Subscribers[typeHash].push_back(std::move(h));

      return token;
    }

    static void Unsubscribe(uint32_t token)
    {
      for (auto& [hash, handlers] : s_Subscribers)
      {
        std::erase_if(handlers, [token](const Handler& h)
          {
            return h.Token == token;
          });
      }
    }

    template<IsEvent T>
    static void Publish(T&& e)
    {
      size_t typeHash = typeid(e).hash_code();

      auto it = s_Subscribers.find(typeHash);
      if (it == s_Subscribers.end())
        return;

      for (auto& handler : it->second)
      {
        if (handler.Callback(e))
          break;
      }
    }
  };
}