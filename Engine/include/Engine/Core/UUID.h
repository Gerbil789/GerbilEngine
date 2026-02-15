#pragma once

#include "Engine/Core/API.h"
#include <cstdint>
#include <functional>

namespace Engine
{
  class ENGINE_API Uuid
  {
  public:
    Uuid(); // generates a new UUID
    Uuid(uint64_t value) : m_Value(value) {}
		Uuid(const Uuid&) = default;

    bool operator==(const Uuid& other) const { return m_Value == other.m_Value; }
    bool operator!=(const Uuid& other) const { return m_Value != other.m_Value; }
		operator uint64_t() const { return m_Value; }

  private:
    uint64_t m_Value{};

  };
}

// Hash specialization so UUID can be used in unordered_map
namespace std
{
  template<>
  struct hash<Engine::Uuid>
  {
    size_t operator()(const Engine::Uuid& uuid) const noexcept
    {
      return static_cast<size_t>(uuid);
    }
  };
}
