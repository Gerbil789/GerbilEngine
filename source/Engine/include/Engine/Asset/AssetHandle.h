#pragma once


#include "Engine/Core/UUID.h"
#include <concepts>

namespace engine
{
  struct AssetHandle
  {
    Uuid id = 0;

    constexpr AssetHandle() = default;
    constexpr explicit AssetHandle(Uuid uuid) : id(uuid) {}

    constexpr explicit operator bool() const { return static_cast<bool>(id); }
    constexpr bool operator==(const AssetHandle& other) const { return id == other.id; }
    constexpr bool operator!=(const AssetHandle& other) const { return id != other.id; }
  };

  struct AssetHasher
  {
    template <std::derived_from<AssetHandle> T>
    std::size_t operator()(const T& handle) const
    {
      return std::hash<Uuid>()(handle.id);
    }
  };
}