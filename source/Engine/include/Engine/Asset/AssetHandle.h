#pragma once

#include "Engine/Core/UUID.h"

namespace engine
{
  // template <typename T>
  // struct AssetHandle
  // {
  //   Uuid id = 0;

  //   constexpr AssetHandle() = default;
  //   constexpr explicit AssetHandle(Uuid uuid) : id(uuid) {}

  //   constexpr bool operator==(const AssetHandle& other) const { return id == other.id; }
  //   constexpr bool operator!=(const AssetHandle& other) const { return id != other.id; }

  //   // explicit operator bool() const { return id != 0; }

  //   T& Get() const;
  // };

  // // Generic hash struct so std::unordered_map<AssetHandle<T>, ...> works automatically
  // struct AssetHasher
  // {
  //   template <typename T>
  //   std::size_t operator()(const AssetHandle<T>& handle) const
  //   {
  //     return std::hash<Uuid>()(handle.id);
  //   }
  // };

  struct AssetHandle
  {
    Uuid id = 0;

    constexpr AssetHandle() = default;
    constexpr explicit AssetHandle(Uuid uuid) : id(uuid) {}

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