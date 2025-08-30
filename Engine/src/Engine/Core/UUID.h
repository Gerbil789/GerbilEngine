#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <functional>

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

      m_High = dis(gen);
      m_Low = dis(gen);
    }
    UUID(uint64_t hi, uint64_t lo) : m_High(hi), m_Low(lo) {}
    UUID(const UUID&) = default;

    bool operator==(const UUID& other) const { return m_High == other.m_High && m_Low == other.m_Low; }
    bool operator!=(const UUID& other) const { return !(*this == other); }

    // For WebGPU encoding (RGBA32Uint)
    std::array<uint32_t, 4> ToRGBA32() const
    {
      return {
          static_cast<uint32_t>(m_Low & 0xFFFFFFFFull),
          static_cast<uint32_t>((m_Low >> 32) & 0xFFFFFFFFull),
          static_cast<uint32_t>(m_High & 0xFFFFFFFFull),
          static_cast<uint32_t>((m_High >> 32) & 0xFFFFFFFFull)
      };
    }

    // UUID string format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    std::string ToString() const
    {
      std::stringstream ss;
      ss << std::hex << std::setfill('0')
        << std::setw(8) << ((m_High >> 32) & 0xFFFFFFFFull) << "-"
        << std::setw(4) << ((m_High >> 16) & 0xFFFFull) << "-"
        << std::setw(4) << (m_High & 0xFFFFull) << "-"
        << std::setw(4) << ((m_Low >> 48) & 0xFFFFull) << "-"
        << std::setw(12) << (m_Low & 0xFFFFFFFFFFFFull);
      return ss.str();
    }

    static UUID FromString(const std::string& str)
    {
      uint64_t hi = 0, lo = 0;
      std::stringstream ss(str);
      char dash;
      uint32_t part1, part2, part3, part4;
      uint64_t part5;
      ss >> std::hex >> part1 >> dash >> part2 >> dash >> part3 >> dash >> part4 >> dash >> part5;
      hi = (uint64_t(part1) << 32) | (uint64_t(part2) << 16) | part3;
      lo = (uint64_t(part4) << 48) | part5;
      return UUID(hi, lo);
    }

  private:
    uint64_t m_High;
    uint64_t m_Low;
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
      const auto& data = uuid.ToRGBA32();
      size_t hash = data[0];
      hash ^= data[1] + 0x9e3779b9 + (hash << 6) + (hash >> 2);
      hash ^= data[2] + 0x9e3779b9 + (hash << 6) + (hash >> 2);
      hash ^= data[3] + 0x9e3779b9 + (hash << 6) + (hash >> 2);
      return hash;
    }
  };
}
