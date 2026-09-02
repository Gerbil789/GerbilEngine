#pragma once

#include "Engine/Core/UUID.h"

namespace engine
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
    engine::Uuid id = 0;

    AssetHandle() = default;
    constexpr explicit AssetHandle(engine::Uuid uuid) : id(uuid) {}

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
  struct hash<engine::AssetHandle<T>>
  {
    std::size_t operator()(const engine::AssetHandle<T>& handle) const
    {
      return handle.id;
    }
  };
}