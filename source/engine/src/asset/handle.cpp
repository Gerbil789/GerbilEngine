#include "enginepch.h"
#include "engine/asset/handle.h"
#include "engine/asset/asset_manager.h"
#include "engine/graphics/texture/texture_2d.h"
#include "engine/graphics/mesh.h"
#include "engine/graphics/shader.h"
#include "engine/graphics/material.h"
#include "engine/audio/audio_clip.h"
#include "engine/core/scene.h"

namespace engine
{
  template <typename T>
  T& Handle<T>::Get() const
  {
    return assetManager::Getasset(*this);
  }

  template struct Handle<Texture2D>;
  template struct Handle<Mesh>;
  template struct Handle<Shader>;
  template struct Handle<Material>;
  template struct Handle<MaterialInstance>;
  template struct Handle<AudioClip>;
  template struct Handle<Scene>;
}