#pragma once

#include "Engine/Graphics/Texture/TextureCube.h"

namespace Engine
{
  struct Environment
  {
    TextureCube EnvironmentMap;
    TextureCube IrradianceMap;
    TextureCube PrefilteredSpecularMap;
  };

  class ENGINE_API EnvironmentBaker
  {
  public:
    static Environment BakeEnvironment(Uuid equirectangularHDR);

  private:
    static TextureCube CreateIrradianceMap(const TextureCube& sourceCubemap);
    static TextureCube CreatePrefilteredSpecularMap(const TextureCube& sourceCubemap);
  };
}