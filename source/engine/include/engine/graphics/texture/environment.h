#pragma once

#include "engine/asset/handle.h"
#include "engine/graphics/texture/texture_cube.h"

namespace engine
{
  struct Environment
  {
    TextureCube EnvironmentMap;
    TextureCube IrradianceMap;
    TextureCube PrefilteredSpecularMap;
  };

  Environment BakeEnvironment(TextureHandle equirectangularTextureHandle);
}