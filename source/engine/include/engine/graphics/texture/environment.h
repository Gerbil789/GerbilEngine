#pragma once

#include "engine/asset/handle.h"
#include "engine/graphics/texture/texture_cube.h"

namespace engine
{
  struct Environment
  {
    textureCube EnvironmentMap;
    textureCube IrradianceMap;
    textureCube PrefilteredSpecularMap;
  };

  class EnvironmentBaker
  {
  public:
    static Environment BakeEnvironment(TextureHandle equirectangularhdr);

  private:
    static textureCube CreateIrradianceMap(const textureCube& sourceCubemap);
    static textureCube CreatePrefilteredSpecularMap(const textureCube& sourceCubemap);
  };
}