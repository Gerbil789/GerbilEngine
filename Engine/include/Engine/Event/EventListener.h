#pragma once

#include "Engine/Core/API.h"
#include <cstdint>

namespace Engine
{
  class ENGINE_API EventListener
  {
  public:
    EventListener() = default;

    explicit EventListener(uint32_t token) : m_Token(token) {}

    EventListener(const EventListener&) = delete;
    EventListener& operator=(const EventListener&) = delete;

    EventListener(EventListener&& other) noexcept
    {
      m_Token = other.m_Token;
      other.m_Token = 0;
    }

    EventListener& operator=(EventListener&& other) noexcept
    {
      if (this != &other)
      {
        Release();
        m_Token = other.m_Token;
        other.m_Token = 0;
      }
      return *this;
    }

    ~EventListener()
    {
      Release();
    }

    void Release();

  private:
    uint32_t m_Token = 0;
  };
}