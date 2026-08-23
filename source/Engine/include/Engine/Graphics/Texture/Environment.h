#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Graphics/Texture/TextureCube.h"

namespace Engine
{
  struct Environment
  {
    TextureCube EnvironmentMap;
    TextureCube IrradianceMap;
    TextureCube PrefilteredSpecularMap;
  };

  class EnvironmentBaker
  {
  public:
    static Environment BakeEnvironment(Texture2D equirectangularHDR);

  private:
    static TextureCube CreateIrradianceMap(const TextureCube& sourceCubemap);
    static TextureCube CreatePrefilteredSpecularMap(const TextureCube& sourceCubemap);
  };
}