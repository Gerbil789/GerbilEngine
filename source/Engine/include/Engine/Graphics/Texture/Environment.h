#pragma once

#include "Engine/Asset/Handle.h"
#include "Engine/Graphics/Texture/TextureCube.h"

namespace engine
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
    static Environment BakeEnvironment(TextureHandle equirectangularHDR);

  private:
    static TextureCube CreateIrradianceMap(const TextureCube& sourceCubemap);
    static TextureCube CreatePrefilteredSpecularMap(const TextureCube& sourceCubemap);
  };
}