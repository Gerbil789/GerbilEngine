#include "enginepch.h"
#include "Engine/Asset/Handle.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Core/Scene.h"

namespace engine
{
  template <typename T>
  T& Handle<T>::Get() const
  {
    return AssetManager::GetAsset(*this);
  }

  template struct Handle<Texture2D>;
  template struct Handle<Mesh>;
  template struct Handle<Shader>;
  template struct Handle<Material>;
  template struct Handle<MaterialInstance>;
  template struct Handle<AudioClip>;
  template struct Handle<Scene>;
}