#pragma once

#include <random>

namespace Engine
{
  class UUID
  {
  public:

    UUID()
    {
      static std::random_device rd;
      static std::mt19937_64 gen(rd());
      static std::uniform_int_distribution<uint64_t> dis;

      do 
      {
        m_Value = dis(gen);
      } while (m_Value == 0); // don’t ever generate the reserved value
    }
    UUID(uint64_t value) : m_Value(value) {}
    UUID(const UUID&) = default;

    bool operator==(const UUID& other) const { return m_Value == other.m_Value; }
    bool operator!=(const UUID& other) const { return m_Value != other.m_Value; }

		bool IsValid() const { return m_Value != 0; }

		operator uint64_t() const { return m_Value; }

  private:
    uint64_t m_Value{};

  };
}

// Hash specialization so UUID can be used in unordered_map
namespace std
{
  template<>
  struct hash<Engine::UUID>
  {
    size_t operator()(const Engine::UUID& uuid) const noexcept
    {
      return std::hash<uint64_t>()(uuid);
    }
  };
}
