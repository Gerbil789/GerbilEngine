#pragma once

#include "Engine/Core/UUID.h"

namespace Engine
{
  class Texture2DAsset;
  class MeshAsset;
  class ShaderAsset;
  class MaterialAsset;
  class AudioClipAsset;
  class SceneAsset;

  template <typename Asset>
  struct AssetHandle
  {
    Engine::Uuid id = 0;

    AssetHandle() = default;
    constexpr explicit AssetHandle(Engine::Uuid uuid) : id(uuid) {}

    operator bool() const { return static_cast<bool>(id); }
    bool operator==(const AssetHandle<Asset>& other) const { return id == other.id; }
    bool operator!=(const AssetHandle<Asset>& other) const { return id != other.id; }
  };

  using Texture2D = AssetHandle<Texture2DAsset>;
  using Mesh = AssetHandle<MeshAsset>;
  using Shader = AssetHandle<ShaderAsset>;
  using Material = AssetHandle<MaterialAsset>;
  using AudioClip = AssetHandle<AudioClipAsset>;
  using Scene = AssetHandle<SceneAsset>;
}

namespace std
{
  template <typename T>
  struct hash<Engine::AssetHandle<T>>
  {
    std::size_t operator()(const Engine::AssetHandle<T>& handle) const
    {
      return handle.id;
    }
  };
}